/*
Copyright 2022 aki27

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include QMK_KEYBOARD_H
#include <stdio.h>
#include "quantum.h"


#define MS_BTN1 KC_MS_BTN1
#define MS_BTN2 KC_MS_BTN2
#define MS_BTN3 KC_MS_BTN3


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [0] = LAYOUT(
        KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,             KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,
        KC_A,    KC_S,    KC_D,    KC_F,    KC_G,             KC_H,    KC_J,    KC_K,    KC_L,    KC_MINS,
        KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,             KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,
                          KC_LALT, KC_LNG2, KC_SPC,           KC_ENT,  KC_LNG1, KC_BSPC
    ),
    [1] = LAYOUT(
        KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC,          KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN,
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,          KC_H,    KC_J,    KC_K,    KC_L,    KC_MINS,
        KC_GRV,  KC_TILD, KC_NUBS, KC_PIPE, XXXXXXX,          KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,
                          KC_LALT, KC_LNG2, KC_SPC,           KC_ENT,  KC_LNG1, KC_BSPC
    ),
    [2] = LAYOUT(
        KC_1,    KC_2,    KC_3,    KC_4,    KC_5,             KC_6,    KC_7,    KC_8,    KC_9,    KC_0,
        KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,            KC_APP,  KC_UP,   KC_EQL,  KC_PLUS, KC_MINS,
        KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,           KC_LEFT, KC_DOWN, KC_RGHT, KC_DOT,  KC_SLSH,
                          KC_LALT, KC_LNG2, KC_SPC,           KC_ENT,  KC_LNG1, KC_BSPC
    ),
    [3] = LAYOUT(
        KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,             KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,
        KC_A,    KC_S,    KC_D,    KC_F,    KC_G,             KC_H,    KC_J,    KC_K,    KC_L,    KC_MINS,
        KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,             KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,
                          KC_LALT, KC_LNG2, KC_SPC,           KC_ENT,  KC_LNG1, KC_BSPC
    ),
    [4] = LAYOUT(
        EE_CLR,  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,          MS_BTN3, MS_BTN2, XXXXXXX, XXXXXXX, XXXXXXX,
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,          XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,          XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                          XXXXXXX, XXXXXXX, XXXXXXX,          MS_BTN1, SCRL_MO, XXXXXXX
    ),
    [5] = LAYOUT(
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,          XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,          XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,          XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                          XXXXXXX, XXXXXXX, XXXXXXX,          XXXXXXX, XXXXXXX, XXXXXXX    ),
    [6] = LAYOUT(
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,          XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,          XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,          XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                          XXXXXXX, XXXXXXX, XXXXXXX,          XXXXXXX, XXXXXXX, XXXXXXX    ),
    [7] = LAYOUT(
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,          XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,          XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,          XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                          XXXXXXX, XXXXXXX, XXXXXXX,          XXXXXXX, XXXXXXX, XXXXXXX    )
};



// Chordal Holdの手の割り当て ('L': 左手, 'R': 右手, '*': 左右どちらとも組み合わせ可)
// CHORDAL_HOLDはVialのビルド設定で定義済み。有効/無効はVialのQMK Settings(Tap)で切り替える
#ifdef CHORDAL_HOLD
const char chordal_hold_layout[MATRIX_ROWS][MATRIX_COLS] PROGMEM = LAYOUT(
    'L', 'L', 'L', 'L', 'L',           'R', 'R', 'R', 'R', 'R',
    'L', 'L', 'L', 'L', 'L',           'R', 'R', 'R', 'R', 'R',
    'L', 'L', 'L', 'L', 'L',           'R', 'R', 'R', 'R', 'R',
                   '*', '*', '*',      '*', '*', '*'
);
#endif

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [0] =   { ENCODER_CCW_CW(KC_PGUP, KC_PGDN) },
    [1] =   { ENCODER_CCW_CW(XXXXXXX, XXXXXXX) },
    [2] =   { ENCODER_CCW_CW(XXXXXXX, XXXXXXX) },
    [3] =   { ENCODER_CCW_CW(XXXXXXX, XXXXXXX) },
    [4] =   { ENCODER_CCW_CW(XXXXXXX, XXXXXXX) },
    [5] =   { ENCODER_CCW_CW(XXXXXXX, XXXXXXX) },
    [6] =   { ENCODER_CCW_CW(XXXXXXX, XXXXXXX) },
    [7] =   { ENCODER_CCW_CW(XXXXXXX, XXXXXXX) },
};
#endif



// RDP対策: 修飾付きキーコード(KC_EXLM, LSFT(kc), LCTL(kc)など)で、
// 修飾キーの押下とキー本体の押下の間に待ちを入れる。
// 間隔が短すぎるとRDP上で修飾キーより先にキーが処理され、Shiftが効かないことがあるため。
#ifndef RDP_MOD_DELAY
#    define RDP_MOD_DELAY 20
#endif

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (IS_QK_MODS(keycode)) {
        // 5bit形式(bit4が右手フラグ)の修飾を8bitのHIDモッドマスクへ変換
        uint8_t m5   = QK_MODS_GET_MODS(keycode);
        uint8_t mods = (m5 & 0x10) ? (uint8_t)((m5 & 0x0F) << 4) : (uint8_t)(m5 & 0x0F);
        uint8_t kc   = QK_MODS_GET_BASIC_KEYCODE(keycode);
        if (record->event.pressed) {
            register_weak_mods(mods);
            wait_ms(RDP_MOD_DELAY);
            register_code(kc);
        } else {
            unregister_code(kc);
            wait_ms(RDP_MOD_DELAY);
            unregister_weak_mods(mods);
        }
        return false;
    }
    return true;
}

bool is_mouse_record_kb(uint16_t keycode, keyrecord_t* record) {
    switch(keycode) {
      case KC_LCTL:
        return true;
      case KC_LSFT:
        return true;
      case SCRL_MO:
        return true;
      default:
        return false;
    }
    return is_mouse_record_user(keycode, record);
} 

#ifdef RGB_MATRIX_ENABLE

// rgb_matrix.cで定義されているがヘッダ未公開のため宣言
void rgb_matrix_update_pwm_buffers(void);

// AML表示用LED (layer0のKC_ENTの位置 = matrix {3, 6})
#define AML_LEDS ((const uint8_t[]){ g_led_config.matrix_co[3][6] })

// AML中は青(明るさはRGB Matrixの設定値)、それ以外は消灯
static uint8_t aml_led_val(void) {
    return layer_state_is(get_auto_mouse_layer()) ? rgb_matrix_get_val() : 0;
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) { 
    int is_layer = get_highest_layer(layer_state|default_layer_state);  
    HSV hsv = {0, 255, rgblight_get_val()};
    if (is_layer == 1) {
      hsv.h = 11; //CORAL
    } else if (is_layer == 2)  {
      hsv.h = 85; //GREEN
    } else if (is_layer == 3)  {
      hsv.h = 43; //YELLOW
    } else if (is_layer == 4)  {
      hsv.h = 0; //RED
    } else if (is_layer == 5)  {
      hsv.h = 127; //PURPLE
    } else if (is_layer == 6)  {
      hsv.h = 64; //CHARTREUSE
    } else if (is_layer == 7)  {
      hsv.h = 224;
    } else {
      hsv.h = 184; //CYAN
    }
    RGB rgb = hsv_to_rgb(hsv);
 
    for (uint8_t i = led_min; i <= led_max; i++) {
        if (HAS_FLAGS(g_led_config.flags[i], 0x02)) {
          rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
        }
    }

    // AML表示用LED: AML中は青で点灯、それ以外は消灯
    uint8_t aml_val = aml_led_val();
    for (uint8_t i = 0; i < sizeof(AML_LEDS); i++) {
        RGB_MATRIX_INDICATOR_SET_COLOR(AML_LEDS[i], 0, 0, aml_val);
    }
    return false;
};

// Vialの lighting "All Off" ではRGB Matrix自体が無効になり、上のindicatorsが呼ばれない。
// その間は「I」「O」のLEDだけを直接書き込んで点灯/消灯させる。
// (無効時のRGB Matrixは消灯後に再描画しないため、書き込んだ色が保持される)
void housekeeping_task_user(void) {
    static uint16_t refresh_timer = 0;
    static int16_t  last_val      = -1;

    if (rgb_matrix_is_enabled() || rgb_matrix_get_suspend_state()) {
        last_val = -1;
        return;
    }

    uint8_t aml_val = aml_led_val();
    // AML状態の変化時は即反映。All Offへ切り替えた直後の全消灯で上書きされる場合に備えて定期的にも書き直す
    if (aml_val != last_val || timer_elapsed(refresh_timer) > 100) {
        for (uint8_t i = 0; i < sizeof(AML_LEDS); i++) {
            rgb_matrix_set_color(AML_LEDS[i], 0, 0, aml_val);
        }
        rgb_matrix_update_pwm_buffers();
        last_val      = aml_val;
        refresh_timer = timer_read();
    }
}

#endif
