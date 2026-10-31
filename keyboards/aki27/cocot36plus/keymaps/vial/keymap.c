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


// 初期キーマップ: Vialで保存した 20261024COCOT_rdp.vil の内容
// (EEPROMの初期化後にこの配置で起動する。TD(n)/MC_nの中身はVial側(EEPROM)の設定)
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [0] = LAYOUT(
        KC_Q,         KC_W,         KC_E,         KC_R,         KC_T,                KC_Y,    KC_U,         KC_I,         KC_O,         KC_P,
        LGUI_T(KC_A), LALT_T(KC_S), LCTL_T(KC_D), LSFT_T(KC_F), KC_G,                KC_H,    LSFT_T(KC_J), LCTL_T(KC_K), LALT_T(KC_L), TD(2),
        KC_Z,         KC_X,         KC_C,         KC_V,         KC_B,                KC_N,    KC_M,         TD(0),        TD(1),        TD(14),
                      LT(5, KC_ESC), LT(2, KC_ENT), LT(7, KC_DEL),                   LT(3, KC_BSPC), LT(4, KC_SPC), KC_TAB
    ),
    [1] = LAYOUT(
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,          XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        XXXXXXX, MS_BTN4, MS_BTN2, MS_BTN1, MS_BTN5,          XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,          XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                          MO(5),   MO(2),   MO(7),            XXXXXXX, XXXXXXX, XXXXXXX
    ),
    [2] = LAYOUT(
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,          XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        KC_F11,  KC_F2,   TD(18),  KC_F4,   KC_F5,            KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,
        KC_F12,  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,          XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                          XXXXXXX, XXXXXXX, XXXXXXX,          XXXXXXX, XXXXXXX, XXXXXXX
    ),
    [3] = LAYOUT(
        KC_PPLS, KC_PMNS, KC_PAST, KC_PSLS, LSFT(KC_MINS),    KC_EQL,  LSFT(KC_5), XXXXXXX, XXXXXXX, XXXXXXX,
        KC_1,    KC_2,    KC_3,    KC_4,    KC_5,             KC_6,    KC_7,    KC_8,    KC_9,    KC_0,
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,          XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                          XXXXXXX, XXXXXXX, XXXXXXX,          XXXXXXX, XXXXXXX, XXXXXXX
    ),
    [4] = LAYOUT(
        KC_LBRC, KC_EQL,  KC_RBRC, KC_NUHS, KC_INT1,          LALT(KC_ESC), LCTL(KC_PGUP), KC_UP,        LCTL(KC_PGDN), LSA(KC_ESC),
        TD(12),  TD(7),   TD(8),   TD(15),  LSFT(KC_4),       KC_HOME,      KC_LEFT,       KC_DOWN,      KC_RGHT,       KC_END,
        KC_SCLN, LSFT(KC_3), LSFT(KC_RBRC), LSFT(KC_NUHS), LSFT(KC_5), KC_APP, LCTL(KC_PPLS), LGUI(KC_TAB), LCTL(KC_PMNS), KC_PSCR,
                          KC_PSCR, KC_APP,  XXXXXXX,          XXXXXXX, XXXXXXX, XXXXXXX
    ),
    [5] = LAYOUT(
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,          XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        XXXXXXX, MS_BTN4, MS_BTN2, MS_BTN1, MS_BTN5,          XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,          XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                          XXXXXXX, XXXXXXX, XXXXXXX,          XXXXXXX, XXXXXXX, XXXXXXX
    ),
    [6] = LAYOUT(
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,          XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,          XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,          XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                          XXXXXXX, XXXXXXX, XXXXXXX,          XXXXXXX, XXXXXXX, XXXXXXX
    ),
    [7] = LAYOUT(
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,          SCRL_IN, AM_TOG,  CPI_SW,  ROT_L15, ROT_R15,
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,          XXXXXXX, MC_6,    MC_7,    XXXXXXX, SCRL_SW,
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,          XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, UG_TOGG,
                          XXXXXXX, XXXXXXX, XXXXXXX,          MC_7,    MC_6,    XXXXXXX
    )
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
    [1] =   { ENCODER_CCW_CW(MS_BTN4, MS_BTN5) },
    [2] =   { ENCODER_CCW_CW(LSA(KC_ESC), LALT(KC_ESC)) },
    [3] =   { ENCODER_CCW_CW(XXXXXXX, XXXXXXX) },
    [4] =   { ENCODER_CCW_CW(XXXXXXX, XXXXXXX) },
    [5] =   { ENCODER_CCW_CW(LCTL(KC_PGUP), LCTL(KC_PGDN)) },
    [6] =   { ENCODER_CCW_CW(XXXXXXX, XXXXXXX) },
    [7] =   { ENCODER_CCW_CW(LCTL(LSFT(KC_TAB)), LCTL(KC_TAB)) },
};
#endif



// ---------------------------------------------------------------------------
// RDP対策
// RDP上では、修飾キーとキー本体のレポートが短い間隔で続くと、修飾キーより先に
// キーが処理されて修飾が効かないことがある。修飾キーの状態が変わった直後のキー押下だけ、
// 一定時間空くまで待ってから送る。
//   - 修飾付きキーコード(KC_EXLM, LSFT(kc), LCTL(kc)など): 修飾→待ち→キー
//   - HRM(mod-tap)のホールド、物理的な修飾キー、LMなど: get_mods()の変化を検出し、
//     直後に押されたキーを待たせる(押されるのが遅ければ待ちは発生しない)
// ---------------------------------------------------------------------------
// 修飾キー押下からキー押下までの最低間隔の初期値(文字の決定に効くので長め)
// 実際の値はRDP_UP/RDP_DNで変更でき、EEPROMに保存される
#ifndef RDP_PRESS_DELAY
#    define RDP_PRESS_DELAY 70
#endif
// 修飾キー解放→次のキー押下、キー押下→そのキーの解放の最低間隔(順序を保つための短い待ち。固定)
#ifndef RDP_RELEASE_DELAY
#    define RDP_RELEASE_DELAY 5
#endif
// RDP_UP/RDP_DNの刻みと上限(ms)
#ifndef RDP_DELAY_STEP
#    define RDP_DELAY_STEP 10
#endif
#ifndef RDP_DELAY_MAX
#    define RDP_DELAY_MAX 200
#endif
// RDP_UP/RDP_DN/RDP_TOGを押したときに、OLEDへ設定値を一時表示する時間(ms)
#ifndef RDP_POPUP_TIME
#    define RDP_POPUP_TIME 2000
#endif

// Vialのカスタムキーコード。vial.jsonのcustomKeycodesの9〜11番目と順番を合わせること
#define RDP_UP QK_KB_8   // 押下側の待ちを+10ms
#define RDP_DN QK_KB_9   // 押下側の待ちを-10ms
#define RDP_TOG QK_KB_10 // 対策のオン/オフ(オフで待ち0ms、QMK標準の動作)

// EEPROM(ユーザー領域)に保存する設定。Vialでの書き込み直しでは消えない
#define RDP_CONFIG_MAGIC 0xA5
typedef union {
    uint32_t raw;
    struct {
        uint8_t magic;   // 初期化済みの印
        uint8_t delay;   // 押下側の待ち(ms)
        uint8_t enabled; // 1: オン, 0: オフ
    };
} rdp_config_t;

static rdp_config_t rdp_config;
static uint32_t     rdp_popup_time;
static bool         rdp_popup_active;

static uint32_t rdp_mod_press_time;   // 最後に修飾キーが押された時刻
static uint32_t rdp_mod_release_time; // 最後に修飾キーが離された時刻
static uint32_t rdp_key_press_time;   // 最後にキーが押された時刻
static uint8_t  rdp_mods_before;      // 処理前の修飾キー状態(post_process_record_userで比較)

void eeconfig_init_user(void) {
    rdp_config.raw     = 0;
    rdp_config.magic   = RDP_CONFIG_MAGIC;
    rdp_config.delay   = RDP_PRESS_DELAY;
    rdp_config.enabled = 1;
    eeconfig_update_user(rdp_config.raw);
}

void keyboard_post_init_user(void) {
    rdp_config.raw = eeconfig_read_user();
    if (rdp_config.magic != RDP_CONFIG_MAGIC || rdp_config.delay > RDP_DELAY_MAX || rdp_config.enabled > 1) {
        eeconfig_init_user();
    }
}

// 直前の修飾キー変化から必要な間隔が空くまで待つ
static void rdp_wait_after_mods(void) {
    uint32_t since_press   = timer_elapsed32(rdp_mod_press_time);
    uint32_t since_release = timer_elapsed32(rdp_mod_release_time);
    uint32_t wait          = 0;
    if (since_press < rdp_config.delay) wait = rdp_config.delay - since_press;
    if (since_release < RDP_RELEASE_DELAY && RDP_RELEASE_DELAY - since_release > wait) wait = RDP_RELEASE_DELAY - since_release;
    if (wait) wait_ms(wait);
}

// 押下とその直後の解放が続けて処理される場合(Permissive Holdで溜められたキーなど)に最低間隔を空ける
static void rdp_wait_before_release(void) {
    uint32_t since = timer_elapsed32(rdp_key_press_time);
    if (since < RDP_RELEASE_DELAY) wait_ms(RDP_RELEASE_DELAY - since);
}

// RDP_UP/RDP_DN/RDP_TOGの処理
static void rdp_change_setting(uint16_t keycode) {
    if (keycode == RDP_UP) {
        rdp_config.delay = (rdp_config.delay + RDP_DELAY_STEP > RDP_DELAY_MAX) ? RDP_DELAY_MAX : rdp_config.delay + RDP_DELAY_STEP;
    } else if (keycode == RDP_DN) {
        rdp_config.delay = (rdp_config.delay < RDP_DELAY_STEP) ? 0 : rdp_config.delay - RDP_DELAY_STEP;
    } else {
        rdp_config.enabled = !rdp_config.enabled;
    }
    eeconfig_update_user(rdp_config.raw);
    rdp_popup_time   = timer_read32();
    rdp_popup_active = true;
}

#ifdef OLED_ENABLE
// cocot36plus.cのOLED表示から呼ばれる(cocot36plus.hで宣言)
// 4行目末尾の常時表示: オン'R' / オフ'-'
char cocot_rdp_indicator(void) {
    return rdp_config.enabled ? 'R' : '-';
}

// RDP_UP/RDP_DN/RDP_TOGを押してからRDP_POPUP_TIMEの間だけ、4行目を設定値の表示に切り替える
bool cocot_rdp_popup(char *buf, uint8_t len) {
    if (!rdp_popup_active) return false;
    if (timer_elapsed32(rdp_popup_time) >= RDP_POPUP_TIME) {
        rdp_popup_active = false;
        return false;
    }
    if (rdp_config.enabled) {
        snprintf(buf, len, "RDP ON   %3ums", (unsigned)rdp_config.delay);
    } else {
        snprintf(buf, len, "RDP OFF  (%3ums)", (unsigned)rdp_config.delay);
    }
    return true;
}
#endif

// 修飾キーそのものの押下か(この場合は待たない)
static bool rdp_is_mod_press(uint16_t keycode, keyrecord_t *record) {
    if (IS_MODIFIER_KEYCODE(keycode)) return true;
#ifndef NO_ACTION_TAPPING
    if (IS_QK_MOD_TAP(keycode) && record->tap.count == 0) return true; // HRMのホールド
#endif
    return false;
}

// Mod-Tap のタップ側で Shift 付き記号を出す(OSはJIS配列前提)。
// Mod-Tap はタップ側に基本キーコードしか持てず、LALT_T(S(KC_6)) と書いても Shift が落ちるため、
// タップ時の処理を横取りして Shift+キー を送る。RDP対策がオンなら同じ待ちを入れて Shift 抜けを防ぐ。
static void rdp_tap_shifted(uint8_t kc) {
    if (!rdp_config.enabled) {
        tap_code16(LSFT(kc));
        return;
    }
    register_weak_mods(MOD_BIT(KC_LSFT));
    rdp_mod_press_time = timer_read32();
    rdp_wait_after_mods(); // = 設定中の押下側の待ち
    register_code(kc);
    wait_ms(RDP_RELEASE_DELAY);
    unregister_code(kc);
    wait_ms(RDP_RELEASE_DELAY);
    unregister_weak_mods(MOD_BIT(KC_LSFT));
    rdp_mod_release_time = timer_read32();
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (keycode == RDP_UP || keycode == RDP_DN || keycode == RDP_TOG) {
        if (record->event.pressed) rdp_change_setting(keycode);
        return false;
    }

    rdp_mods_before = get_mods();

    if (rdp_config.enabled) {
        if (record->event.pressed) {
            if (!rdp_is_mod_press(keycode, record)) {
                rdp_wait_after_mods();
            }
        } else {
            rdp_wait_before_release();
        }
    }

    // Mod-Tapのタップで Shift 付き記号(RDP対策のオン/オフに関わらず有効)
    switch (keycode) {
        case LALT_T(KC_6): // ホールド:Alt   / タップ:＆（Shift+6）
        case LCTL_T(KC_8): // ホールド:Ctrl  / タップ:（（Shift+8）
        case LSFT_T(KC_9): // ホールド:Shift / タップ:）（Shift+9）
            if (record->tap.count) {
                if (record->event.pressed) rdp_tap_shifted(QK_MOD_TAP_GET_TAP_KEYCODE(keycode));
                return false; // タップの解放も横取り(基本キーの解放を送らない)
            }
            break; // ホールドは通常の修飾キーとして処理
    }

    if (!rdp_config.enabled) return true; // オフ: QMK標準の動作

    if (IS_QK_MODS(keycode)) {
        // 5bit形式(bit4が右手フラグ)の修飾を8bitのHIDモッドマスクへ変換
        uint8_t m5   = QK_MODS_GET_MODS(keycode);
        uint8_t mods = (m5 & 0x10) ? (uint8_t)((m5 & 0x0F) << 4) : (uint8_t)(m5 & 0x0F);
        uint8_t kc   = QK_MODS_GET_BASIC_KEYCODE(keycode);
        if (record->event.pressed) {
            register_weak_mods(mods);
            rdp_mod_press_time = timer_read32();
            rdp_wait_after_mods(); // = 設定中の押下側の待ち
            register_code(kc);
            rdp_key_press_time = timer_read32();
        } else {
            unregister_code(kc);
            wait_ms(RDP_RELEASE_DELAY);
            unregister_weak_mods(mods);
            rdp_mod_release_time = timer_read32();
        }
        return false;
    }

    if (record->event.pressed) {
        rdp_key_press_time = timer_read32();
    }
    return true;
}

// キー処理の後で修飾キーの状態を比較し、押された/離された時刻を記録する。
// mod-tapのホールド(Hold On Other Key Pressで処理中にホールドへ変わる場合を含む)、
// 物理的な修飾キー、LM(layer, mod)などをまとめて検出できる。
void post_process_record_user(uint16_t keycode, keyrecord_t *record) {
    uint8_t mods_after = get_mods();
    if (mods_after & ~rdp_mods_before) rdp_mod_press_time = timer_read32();
    if (rdp_mods_before & ~mods_after) rdp_mod_release_time = timer_read32();
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

// RDP対策表示用LED (左親指の一番中心寄り、layer0のLT(7, KC_DEL)の位置 = matrix {3, 3})
#define RDP_LED (g_led_config.matrix_co[3][3])

// PCのスリープ中はAML・RDPの表示用LEDを消灯する
// (RGB_MATRIX_SLEEPは無効なので、RGB Matrix側ではスリープ中として扱われない)
static bool    indicator_leds_suspended = false;
static int32_t indicator_leds_last_state = -1; // All Off時に最後に書き込んだ状態(-1: 未書き込み)

// AML中は青(明るさはRGB Matrixの設定値)、それ以外とスリープ中は消灯
static uint8_t aml_led_val(void) {
    if (indicator_leds_suspended) return 0;
    return layer_state_is(get_auto_mouse_layer()) ? rgb_matrix_get_val() : 0;
}

// RDP対策オンは白(明るさはRGB Matrixの設定値)、オフとスリープ中は消灯
static uint8_t rdp_led_val(void) {
    if (indicator_leds_suspended) return 0;
    return rdp_config.enabled ? rgb_matrix_get_val() : 0;
}

// 表示用LEDへ直接書き込む(RGB Matrix無効時・スリープ時用)
static void write_indicator_leds(uint8_t aml_val, uint8_t rdp_val) {
    for (uint8_t i = 0; i < sizeof(AML_LEDS); i++) {
        rgb_matrix_set_color(AML_LEDS[i], 0, 0, aml_val);
    }
    rgb_matrix_set_color(RDP_LED, rdp_val, rdp_val, rdp_val);
    rgb_matrix_update_pwm_buffers();
}

// スリープに入るとき: 表示用LEDのバッファを0にするだけで、ここでは送信(update_pwm_buffers)しない。
// (スリープ処理中にここで送信すると、OLEDが消えず復帰後に操作できなくなる不具合が出たため)
// 実際の消灯は、この直後にQMKが行うRGB_MATRIX_SLEEPの全消灯(描画+送信)に任せる。
void suspend_power_down_user(void) {
    if (!indicator_leds_suspended) {
        indicator_leds_suspended = true;
        for (uint8_t i = 0; i < sizeof(AML_LEDS); i++) {
            rgb_matrix_set_color(AML_LEDS[i], 0, 0, 0);
        }
        rgb_matrix_set_color(RDP_LED, 0, 0, 0);
    }
}

void suspend_wakeup_init_user(void) {
    indicator_leds_suspended  = false;
    indicator_leds_last_state = -1; // 復帰後に状態を書き直させる
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

    // RDP対策表示用LED: オンは白で点灯、オフは消灯
    uint8_t rdp_val = rdp_led_val();
    RGB_MATRIX_INDICATOR_SET_COLOR(RDP_LED, rdp_val, rdp_val, rdp_val);
    return false;
};

// Vialの lighting "All Off" ではRGB Matrix自体が無効になり、上のindicatorsが呼ばれない。
// その間はAML表示用・RDP対策表示用のLEDだけを直接書き込んで点灯/消灯させる。
// (無効時のRGB Matrixは消灯後に再描画しないため、書き込んだ色が保持される)
void housekeeping_task_user(void) {
    static uint16_t refresh_timer = 0;

    if (rgb_matrix_is_enabled() || rgb_matrix_get_suspend_state() || indicator_leds_suspended) {
        indicator_leds_last_state = -1;
        return;
    }

    uint8_t aml_val = aml_led_val();
    uint8_t rdp_val = rdp_led_val();
    int32_t state   = ((int32_t)rdp_val << 8) | aml_val;
    // 状態の変化時は即反映。All Offへ切り替えた直後の全消灯で上書きされる場合に備えて定期的にも書き直す
    if (state != indicator_leds_last_state || timer_elapsed(refresh_timer) > 100) {
        write_indicator_leds(aml_val, rdp_val);
        indicator_leds_last_state = state;
        refresh_timer             = timer_read();
    }
}

#endif
