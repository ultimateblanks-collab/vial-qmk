/* Copyright 2020 Alexander Tulloh
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "quantum.h"
#include <math.h>
#include "cocot36plus.h"
#include "wait.h"
#include "debug.h"
#include <stdio.h>
#include "pointing_device.h"
extern const pointing_device_driver_t pointing_device_driver;


// Invert vertical scroll direction
#ifndef COCOT_SCROLL_INV_DEFAULT
#    define COCOT_SCROLL_INV_DEFAULT true
#endif

#ifndef COCOT_CPI_OPTIONS
#    define COCOT_CPI_OPTIONS {700, 800, 900, 1000, 1200 }
#    ifndef COCOT_CPI_DEFAULT
#       define COCOT_CPI_DEFAULT 1
#    endif
#endif
#ifndef COCOT_CPI_DEFAULT
#    define COCOT_CPI_DEFAULT 1
#endif

#ifndef COCOT_SCROLL_DIVIDERS
#    define COCOT_SCROLL_DIVIDERS { 4, 5, 6, 7, 8, 9 }
#    ifndef COCOT_SCROLL_DIV_DEFAULT
#       define COCOT_SCROLL_DIV_DEFAULT 3
#    endif
#endif
#ifndef COCOT_SCROLL_DIV_DEFAULT
#    define COCOT_SCROLL_DIV_DEFAULT 3
#endif


#ifndef COCOT_ROTATION_ANGLE
#    define COCOT_ROTATION_ANGLE { -90, -75, -60, -45, -30, -15, 0, 15, 30, 45, 60, 75, 90 }
#    ifndef COCOT_ROTATION_DEFAULT
#       define COCOT_ROTATION_DEFAULT 6
#    endif
#endif
#ifndef COCOT_ROTATION_DEFAULT
#    define COCOT_ROTATION_DEFAULT 6
#endif
#ifndef COCOT_AUTO_MOUSE_MODE
#    define COCOT_AUTO_MOUSE_MODE true
#endif

// AML中に押してもAMLを維持するキーポジション {row, col}
#ifndef COCOT_AML_KEEP_POSITIONS
#    define COCOT_AML_KEEP_POSITIONS { {0, 5}, {0, 6}, {3, 6}, {3, 7} }
#endif
// マウスボタン入力からdefault layerへ戻るまでの時間(ms)
#ifndef COCOT_AML_CLICK_EXIT_TIME
#    define COCOT_AML_CLICK_EXIT_TIME 250
#endif
// このレイヤーが有効な間はCPIを 設定値/COCOT_SLOW_CPI_DIV に落とす
#ifndef COCOT_SLOW_CPI_LAYER
#    define COCOT_SLOW_CPI_LAYER 5
#endif
#ifndef COCOT_SLOW_CPI_DIV
#    define COCOT_SLOW_CPI_DIV 4
#endif





cocot_config_t cocot_config;
uint16_t cpi_array[] = COCOT_CPI_OPTIONS;
uint16_t scrl_div_array[] = COCOT_SCROLL_DIVIDERS;
uint16_t angle_array[] = COCOT_ROTATION_ANGLE;
#define CPI_OPTION_SIZE (sizeof(cpi_array) / sizeof(uint16_t))
#define SCRL_DIV_SIZE (sizeof(scrl_div_array) / sizeof(uint16_t))
#define ANGLE_SIZE (sizeof(angle_array) / sizeof(uint16_t))

// 設定値のCPIをセンサーへ反映 (COCOT_SLOW_CPI_LAYERが有効な間は1/COCOT_SLOW_CPI_DIV)
static void cocot_apply_cpi(layer_state_t state) {
    static uint16_t applied_cpi = 0;
    uint16_t cpi = cpi_array[cocot_config.cpi_idx];
    if (layer_state_cmp(state, COCOT_SLOW_CPI_LAYER)) {
        cpi /= COCOT_SLOW_CPI_DIV;
    }
    if (cpi != applied_cpi) {
        pointing_device_set_cpi(cpi);
        applied_cpi = cpi;
    }
}

void pointing_device_init_kb(void) {
    // set the CPI.
    cocot_apply_cpi(layer_state);
    cocot_config.raw = eeconfig_read_kb();
    eeconfig_update_kb(cocot_config.raw);
    //set_auto_mouse_layer(4);
    set_auto_mouse_enable(cocot_config.auto_mouse);
}

report_mouse_t pointing_device_task_kb(report_mouse_t mouse_report) {
    static float x_accumulator = 0.0;
    static float y_accumulator = 0.0;
    static float prev_x = 0.0, prev_y = 0.0;

    float sensitivity = 0.5;           // Movement sensitivity
    float smoothing_factor = 0.7;     // Smoothing factor
    float sensitivity_multiplier = 1.5; // Base sensitivity multiplier

    // Apply rotation angle adjustment
    double rad = angle_array[cocot_config.rotation_angle] * (M_PI / 180) * -1;
    float rotated_x = -(mouse_report.x * cos(rad) - mouse_report.y * sin(rad)); // Reverse X-direction
    float rotated_y = mouse_report.x * sin(rad) + mouse_report.y * cos(rad);

    // Apply smoothing to the rotated values
    float smoothed_x = prev_x * smoothing_factor + rotated_x * (1.0 - smoothing_factor);
    float smoothed_y = prev_y * smoothing_factor + rotated_y * (1.0 - smoothing_factor);
    prev_x = smoothed_x;
    prev_y = smoothed_y;

    // Calculate the magnitude of movement
    float movement_magnitude = sqrt(smoothed_x * smoothed_x + smoothed_y * smoothed_y);

    // Dynamic multiplier: slower for small movements, faster for large
    float dynamic_multiplier = 1.0 + movement_magnitude / 10.0; // Adjust divisor for desired scaling
    dynamic_multiplier = fmin(fmax(dynamic_multiplier, 0.5), 3.0); // Clamp between 0.5 and 3.0

    // Apply dynamic multiplier to smoothed values
    smoothed_x *= sensitivity_multiplier * dynamic_multiplier;
    smoothed_y *= sensitivity_multiplier * dynamic_multiplier;

    // Scroll mode handling
    if (cocot_get_scroll_mode()) {
        static int h_acm = 0, v_acm = 0;

        // Determine scroll direction
        if (abs((int)smoothed_x) > abs((int)smoothed_y)) {
            smoothed_y = 0; // Horizontal scroll
        } else {
            smoothed_x = 0; // Vertical scroll
        }

        // Accumulate scroll values
        if (cocot_config.scrl_inv) {
            h_acm += smoothed_x;
            v_acm -= smoothed_y;
        } else {
            h_acm -= smoothed_x;
            v_acm += smoothed_y;
        }

        // Calculate scroll values with division factor
        int8_t h_scroll = h_acm >> scrl_div_array[cocot_config.scrl_div];
        int8_t v_scroll = v_acm >> scrl_div_array[cocot_config.scrl_div];

        // Apply scroll to mouse report
        if (h_scroll != 0) {
            mouse_report.h += h_scroll;
            h_acm -= h_scroll << scrl_div_array[cocot_config.scrl_div];
        }
        if (v_scroll != 0) {
            mouse_report.v += v_scroll;
            v_acm -= v_scroll << scrl_div_array[cocot_config.scrl_div];
        }

        // Reset X/Y movement in scroll mode
        mouse_report.x = 0;
        mouse_report.y = 0;
    } else {
        // Movement smoothing and accumulation for normal mode
        x_accumulator += smoothed_x * sensitivity;
        y_accumulator += smoothed_y * sensitivity;

        if (fabs(x_accumulator) >= 1.0) {
            mouse_report.x = (int8_t)x_accumulator;
            x_accumulator -= mouse_report.x;
        } else {
            mouse_report.x = 0;
        }

        if (fabs(y_accumulator) >= 1.0) {
            mouse_report.y = (int8_t)y_accumulator;
            y_accumulator -= mouse_report.y;
        } else {
            mouse_report.y = 0;
        }
    }

    return pointing_device_task_user(mouse_report);
}




#ifdef POINTING_DEVICE_AUTO_MOUSE_ENABLE
static const uint8_t aml_keep_positions[][2] = COCOT_AML_KEEP_POSITIONS;
#define AML_KEEP_POSITIONS_SIZE (sizeof(aml_keep_positions) / sizeof(aml_keep_positions[0]))

static bool is_aml_keep_position(keypos_t key) {
    for (uint8_t i = 0; i < AML_KEEP_POSITIONS_SIZE; i++) {
        if (aml_keep_positions[i][0] == key.row && aml_keep_positions[i][1] == key.col) return true;
    }
    return false;
}

// AMLレイヤーが自動で有効になっている状態か (TG/TO等でトグルされている場合は除く)
static bool is_aml_layer_active(void) {
    return get_auto_mouse_enable() && layer_state_is(get_auto_mouse_layer()) && !get_auto_mouse_toggle();
}

// マウスボタン入力後、COCOT_AML_CLICK_EXIT_TIME経過でdefault layerへ戻すためのタイマー
static bool     aml_click_exit_pending = false;
static uint16_t aml_click_exit_timer   = 0;

void housekeeping_task_kb(void) {
    if (aml_click_exit_pending && timer_elapsed(aml_click_exit_timer) > COCOT_AML_CLICK_EXIT_TIME) {
        aml_click_exit_pending = false;
        if (is_aml_layer_active()) {
            // レイヤーを落として状態をリセット。
            // reset_triggerが開始するdelayタイマーはkeyevent(true/false)で解除し(tracker増減は相殺)、
            // クリック後にボールを動かしている場合はすぐAMLへ戻れるようにする。
            auto_mouse_reset_trigger(true);
            auto_mouse_keyevent(true);
            auto_mouse_keyevent(false);
        }
    }
    housekeeping_task_user();
}
#endif

#ifdef POINTING_DEVICE_AUTO_MOUSE_ENABLE
// AML解除用に握りつぶした(press/releaseとも処理しない)キーの位置
static matrix_row_t aml_swallowed_keys[MATRIX_ROWS];
#endif

bool pre_process_record_kb(uint16_t keycode, keyrecord_t* record) {
#ifdef POINTING_DEVICE_AUTO_MOUSE_ENABLE
    keypos_t key = record->event.key;
    if (IS_EVENT(record->event) && key.row < MATRIX_ROWS && key.col < MATRIX_COLS) {
        matrix_row_t bit = (matrix_row_t)1 << key.col;
        if (record->event.pressed && is_aml_layer_active()) {
            if (!is_aml_keep_position(key)) {
                // 指定ポジション以外のキーが押されたら即default layerへ戻す。
                // キーコード確定(process_record_quantum)より前にレイヤーを落とすので、
                // 押したキーはAMLレイヤーではなくdefault layer側のキーとして入力される。
                // auto_mouse_reset_trigger()でdelayタイマーも開始されるため、ここから再発動まで待ちが入る。
                auto_mouse_reset_trigger(true);
            } else if (keycode == KC_NO) {
                // 指定ポジションでAMLレイヤー側がKC_NOなら、何も入力せずAMLだけ解除する。
                // QMK標準の解除に任せると、process_record_quantum中にレイヤーが落ちた後で
                // actionが決まるため、default layer側のキー(例: LGUI_T(KC_A))が押されてしまう。
                auto_mouse_reset_trigger(true);
                aml_swallowed_keys[key.row] |= bit;
                return false;
            }
        } else if (!record->event.pressed && (aml_swallowed_keys[key.row] & bit)) {
            aml_swallowed_keys[key.row] &= ~bit;
            return false;
        }
    }
#endif
    return pre_process_record_user(keycode, record);
}

bool process_record_kb(uint16_t keycode, keyrecord_t* record) {
    // xprintf("KL: kc: %u, col: %u, row: %u, pressed: %u\n", keycode, record->event.key.col, record->event.key.row, record->event.pressed);

    if (!process_record_user(keycode, record)) return false;

#ifdef POINTING_DEVICE_AUTO_MOUSE_ENABLE
    // マウスボタンを押したらCOCOT_AML_CLICK_EXIT_TIME後にdefault layerへ戻す。
    // 押すたびにタイマーを再スタートするので、ダブルクリックの2打目もAMLレイヤー上で入力される。
    if (record->event.pressed && keycode >= KC_MS_BTN1 && keycode <= KC_MS_BTN8 && is_aml_layer_active()) {
        aml_click_exit_pending = true;
        aml_click_exit_timer   = timer_read();
    }
#endif

    switch (keycode) {
        #ifndef MOUSEKEY_ENABLE
                // process KC_MS_BTN1~8 by myself
                // See process_action() in quantum/action.c for details.
                case KC_MS_BTN1 ... KC_MS_BTN8: {
                    extern void register_button(bool, enum mouse_buttons);
                    register_button(record->event.pressed, MOUSE_BTN_MASK(keycode - KC_MS_BTN1));
                    return false;
                }
        #endif
        //*
        case AM_TOG:
            if(record->event.pressed) { // key down
                //auto_mouse_layer_off(); // disable target layer if needed
                cocot_config.auto_mouse ^= 1;
                eeconfig_update_kb(cocot_config.raw);
                set_auto_mouse_enable(cocot_config.auto_mouse);
                //auto_mouse_tg_off = !get_auto_mouse_enable();
            } // do nothing on key up
            return false; // prevent further processing of keycode            
    //*/
    }
    
    if (keycode == CPI_SW && record->event.pressed) {
        cocot_config.cpi_idx = (cocot_config.cpi_idx + 1) % CPI_OPTION_SIZE;
        eeconfig_update_kb(cocot_config.raw);
        cocot_apply_cpi(layer_state);
    }

    if (keycode == SCRL_SW && record->event.pressed) {
        cocot_config.scrl_div = (cocot_config.scrl_div + 1) % SCRL_DIV_SIZE;
        eeconfig_update_kb(cocot_config.raw);
    }
    
    if (keycode == ROT_R15 && record->event.pressed) {
        cocot_config.rotation_angle = (cocot_config.rotation_angle + 1) % ANGLE_SIZE;
        eeconfig_update_kb(cocot_config.raw);
    }

    if (keycode == ROT_L15 && record->event.pressed) {
        cocot_config.rotation_angle = (ANGLE_SIZE + cocot_config.rotation_angle - 1) % ANGLE_SIZE;
        eeconfig_update_kb(cocot_config.raw);
    }

    if (keycode == SCRL_IN && record->event.pressed) {
        cocot_config.scrl_inv ^= 1;
        eeconfig_update_kb(cocot_config.raw);
    }

    if (keycode == SCRL_TO && record->event.pressed) {
        { cocot_config.scrl_mode ^= 1; }
    }

    if (keycode == SCRL_MO) {
        { cocot_config.scrl_mode ^= 1; }
    }

    return true;
}
/*
layer_state_t layer_state_set_kb(layer_state_t state) {
    switch(get_highest_layer(state)){
        case 1 ... 2:
            //rgblight_sethsv_range(HSV_YELLOW, 0, 9);
            cocot_set_scroll_mode(true);
            break;
        case 3 ... 7:
            //rgblight_sethsv_range(HSV_CYAN, 0, 9);
            cocot_set_scroll_mode(false);
            set_auto_mouse_enable(true);
            break;
        default:
            //rgblight_sethsv_range(HSV_RED, 0, 9);
            cocot_set_scroll_mode(false);
            set_auto_mouse_enable(true);
            break;
        }
    //rgblight_set_effect_range( 9, 36);
  return state;
};
*/

layer_state_t layer_state_set_kb(layer_state_t state) {
    switch(get_highest_layer(remove_auto_mouse_layer(state, true))) {
        case 1 ... 2:
            //rgblight_sethsv_range(HSV_YELLOW, 0, 9);
            cocot_set_scroll_mode(true);
            state = remove_auto_mouse_layer(state, false);
            set_auto_mouse_enable(false);
            break;
        case 3 ... 7:
            //rgblight_sethsv_range(HSV_CYAN, 0, 9);
            cocot_set_scroll_mode(false);
            //set_auto_mouse_enable(true);
            break;
        default:
            //rgblight_sethsv_range(HSV_RED, 0, 9);
            cocot_set_scroll_mode(false);
            
            if (cocot_config.auto_mouse) {
                set_auto_mouse_enable(true);
            } else {
                //state = remove_auto_mouse_layer(state, false);
                set_auto_mouse_enable(false);
            }
            
            //set_auto_mouse_enable(true);
            //state = remove_auto_mouse_layer(state, false);
            //set_auto_mouse_enable(cocot_config.auto_mouse);
            break;
        }
    //rgblight_set_effect_range( 9, 36);
    cocot_apply_cpi(state);
  return state;
};



void eeconfig_init_kb(void) {
    cocot_config.cpi_idx = COCOT_CPI_DEFAULT;
    cocot_config.scrl_div = COCOT_SCROLL_DIV_DEFAULT;
    cocot_config.rotation_angle = COCOT_ROTATION_DEFAULT;
    cocot_config.scrl_inv = COCOT_SCROLL_INV_DEFAULT;
    cocot_config.scrl_mode = false;
    cocot_config.auto_mouse = COCOT_AUTO_MOUSE_MODE;
    eeconfig_update_kb(cocot_config.raw);
    eeconfig_init_user();
}


void matrix_init_kb(void) {
    // is safe to just read CPI setting since matrix init
    // comes before pointing device init.
    cocot_config.raw = eeconfig_read_kb();
    if (cocot_config.cpi_idx > CPI_OPTION_SIZE) // || cocot_config.scrl_div > SCRL_DIV_SIZE || cocot_config.rotation_angle > ANGLE_SIZE)
    {
        eeconfig_init_kb();
    }
    matrix_init_user();
}


bool cocot_get_scroll_mode(void) {
    return cocot_config.scrl_mode;
}

void cocot_set_scroll_mode(bool mode) {
    cocot_config.scrl_mode = mode;
}






#ifdef RGB_MATRIX_ENABLE
    led_config_t g_led_config = { {
    // Key Matrix to LED Index
        {  9,10,11,12,13,40,41,42,43,44 },
        { 14,15,16,17,18,35,36,37,38,39 },
        { 19,20,21,22,23,30,31,32,33,34 },
        { NO_LED,24,25,26,NO_LED,NO_LED,27,28,29,NO_LED}
    }, {
    // LED Index to Physical Position
        // Underglow
        { 132,  10 }, { 180,  10 }, { 180,  50 }, { 132,  50 }, { 108,  50 }, { 84,  50 }, { 12,  50 }, { 12,  10 }, { 84,  10 },
        // Per Key Backlight
        // Left Side
        {   0,   0 }, {  24,   0 }, {  48,   0 }, {  72,   0 }, {  96,   0 },
        {   0,  20 }, {  24,  20 }, {  48,  20 }, {  72,  20 }, {  96,  20 },
        {   0,  40 }, {  24,  40 }, {  48,  40 }, {  72,  40 }, {  96,  40 },
                                    {  48,  60 }, {  72,  60 }, {  96,  60 },
        // Right Side
        { 120,  60 }, { 144,  60 }, { 168,  60 },
        { 120,  40 }, { 144,  40 }, { 168,  40 }, { 192,  40 }, { 216,  40 },
        { 120,  20 }, { 144,  20 }, { 168,  20 }, { 192,  20 }, { 216,  20 },
        { 120,   0 }, { 144,   0 }, { 168,   0 }, { 192,   0 }, { 216,   0 }
    }, {
    // LED Index to Flag
        2, 2, 2, 2, 2, 2, 2, 2, 2,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4
} };
#endif

// OLED utility

#ifdef OLED_ENABLE

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    return OLED_ROTATION_0;
}


static const char PROGMEM cocot_logo[] = {
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x92, 0x93, 0x94,
    0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1, 0xB2, 0xB3, 0xB4,
    0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 0xD0, 0xD1, 0xD2, 0xD3, 0xD4,
    0};

void render_logo(void) {
    oled_write_P(cocot_logo, false);
};



void oled_write_layer_state(void) {

    oled_write_P(PSTR("Layer"), false);
    // int cpi = pointing_device_get_cpi();
    int cpi = cpi_array[cocot_config.cpi_idx];
    int scroll_div = scrl_div_array[cocot_config.scrl_div];
    int angle_origin = angle_array[cocot_config.rotation_angle];
    int angle;
    if (angle_origin > 360) {
        angle = angle_origin - 65536;
    } else {
        angle = angle_origin;
    }
    int lyr = get_highest_layer(layer_state);

    char buf1[6];
    char buf2[6];
    char buf3[8];
    char buf4[8];

    snprintf(buf1, 6, "%4d", cpi);
    snprintf(buf2, 6, "%1d", scroll_div);
    snprintf(buf3, 8, "%2d", abs(angle));
    snprintf(buf4, 8, "%1d", lyr);
    
    oled_write(buf4, false);
    //oled_write_P(get_u8_str(get_highest_layer(layer_state), '0'), false);
    
    oled_write_P(PSTR("/"), false);
        if (cocot_get_scroll_mode()){
        oled_write_P(PSTR("S"), false);
    } else{
        oled_write_P(PSTR("C"), false);
    }

    oled_write_P(PSTR("/"), false);
    if (get_auto_mouse_enable()){
        oled_write_P(PSTR("Y"), false);
    } else{
        oled_write_P(PSTR("N"), false);
    }
    
    oled_write_P(PSTR("/"), false);
    oled_write(buf1, false);
    oled_write_P(PSTR("/"), false);
    
    oled_write(buf2, false);

    oled_write_P(PSTR("/"), false);
    if (angle < 0) {
        oled_write_P(PSTR("-"), false);
    } else {
        oled_write_P(PSTR(" "), false);
    }
    oled_write(buf3, false);
}


bool oled_task_user(void) {
    render_logo();
    oled_write_layer_state();
    return false;
}


#endif
