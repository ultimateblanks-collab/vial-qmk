/*
Copyright 2022 @Yowkees
Copyright 2022 MURAOKA Taro (aka KoRoN, @kaoriya)

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

#include "quantum.h"
#include "transactions.h"
#include <string.h>
#include <stdio.h>
#ifdef RGBLIGHT_ENABLE
#    include "ws2812.h"
#    include "rgblight_drivers.h"
#endif

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  // keymap for default (Vial)
  [0] = LAYOUT_universal(
    KC_Q     , KC_W     , KC_E     , KC_R     , KC_T     ,                            KC_Y     , KC_U     , KC_I     , KC_O     , KC_P     ,
    KC_A     , KC_S     , KC_D     , KC_F     , KC_G     ,                            KC_H     , KC_J     , KC_K     , KC_L     , KC_MINS  ,
    KC_Z     , KC_X     , KC_C     , KC_V     , KC_B     ,                            KC_N     , KC_M     , KC_COMM  , KC_DOT   , KC_SLSH  ,
    KC_LCTL  , KC_LGUI  , KC_LALT  ,LSFT_T(KC_LNG2),LT(1,KC_SPC),LT(3,KC_LNG1),KC_BSPC,LT(2,KC_ENT),LSFT_T(KC_LNG2),KC_RALT,KC_RGUI, KC_RSFT
  ),

  [1] = LAYOUT_universal(
    KC_F1    , KC_F2    , KC_F3    , KC_F4    , KC_RBRC  ,                            KC_F6    , KC_F7    , KC_F8    , KC_F9    , KC_F10   ,
    KC_F5    , KC_EXLM  , S(KC_6)  ,S(KC_INT3), S(KC_8)  ,                           S(KC_INT1), KC_BTN1  , KC_PGUP  , KC_BTN2  , KC_SCLN  ,
    S(KC_EQL),S(KC_LBRC),S(KC_7)   , S(KC_2)  ,S(KC_RBRC),                            KC_LBRC  , KC_DLR   , KC_PGDN  , KC_BTN3  , KC_F11   ,
    KC_INT1  , KC_EQL   , S(KC_3)  , _______  , _______  , _______  ,      TO(2)    , TO(0)    , _______  , KC_RALT  , KC_RGUI  , KC_F12
  ),

  [2] = LAYOUT_universal(
    KC_TAB   , KC_7     , KC_8     , KC_9     , KC_MINS  ,                            KC_NUHS  , _______  , KC_BTN3  , _______  , KC_BSPC  ,
   S(KC_QUOT), KC_4     , KC_5     , KC_6     ,S(KC_SCLN),                            S(KC_9)  , KC_BTN1  , KC_UP    , KC_BTN2  , KC_QUOT  ,
    KC_SLSH  , KC_1     , KC_2     , KC_3     ,S(KC_MINS),                           S(KC_NUHS), KC_LEFT  , KC_DOWN  , KC_RGHT  , _______  ,
    KC_ESC   , KC_0     , KC_DOT   , KC_DEL   , KC_ENT   , KC_BSPC  ,      _______  , _______  , _______  , _______  , _______  , _______
  ),

  [3] = LAYOUT_universal(
    RGB_TOG  , AML_TO   , AML_I50  , AML_D50  , _______  ,                            _______  , _______  , SSNP_HOR , SSNP_VRT , SSNP_FRE ,
    RGB_MOD  , RGB_HUI  , RGB_SAI  , RGB_VAI  , SCRL_DVI ,                            _______  , _______  , _______  , _______  , _______  ,
    RGB_RMOD , RGB_HUD  , RGB_SAD  , RGB_VAD  , SCRL_DVD ,                            CPI_D1K  , CPI_D100 , CPI_I100 , CPI_I1K  , KBC_SAVE ,
    QK_BOOT  , KBC_RST  , _______  , _______  , _______  , _______  ,      _______  , _______  , _______  , _______  , KBC_RST  , QK_BOOT
  ),

  [4] = LAYOUT_universal(
    _______  , _______  , _______  , _______  , _______  ,                            _______  , _______  , _______  , _______  , _______  ,
    _______  , _______  , _______  , _______  , _______  ,                            _______  , _______  , _______  , _______  , _______  ,
    _______  , _______  , _______  , _______  , _______  ,                            _______  , _______  , _______  , _______  , _______  ,
    _______  , _______  , _______  , _______  , _______  , _______  ,      _______  , _______  , _______  , _______  , _______  , _______
  ),

  [5] = LAYOUT_universal(
    _______  , _______  , _______  , _______  , _______  ,                            _______  , _______  , _______  , _______  , _______  ,
    _______  , _______  , _______  , _______  , _______  ,                            _______  , _______  , _______  , _______  , _______  ,
    _______  , _______  , _______  , _______  , _______  ,                            _______  , _______  , _______  , _______  , _______  ,
    _______  , _______  , _______  , _______  , _______  , _______  ,      _______  , _______  , _______  , _______  , _______  , _______
  ),
};
// clang-format on

layer_state_t layer_state_set_user(layer_state_t state) {
    // Auto enable scroll mode when the highest layer is 3
    keyball_set_scroll_mode(get_highest_layer(state) == 3);
    return state;
}

//////////////////////////////////////////////////////////////////////////////
// 低速CPIレイヤー

static int16_t slow_divmod16(int16_t *v, int16_t div) {
    int16_t r = *v / div;
    *v -= r * div;
    return r;
}

static inline int8_t slow_clip2int8(int16_t v) {
    return (v) < -127 ? -127 : (v) > 127 ? 127 : (int8_t)v;
}

// keyball.c の既定処理 (Keyball39用) に、KEYBALL_SLOW_CPI_LAYER 有効中の 1/KEYBALL_SLOW_CPI_DIV を追加したもの。
// 割り切れなかった移動量は m に残して次回へ持ち越すので、CPIを落とした場合と同じ動きになる。
void keyball_on_apply_motion_to_mouse_move(keyball_motion_t *m, report_mouse_t *r, bool is_left) {
    int16_t x, y;
    if (layer_state_is(KEYBALL_SLOW_CPI_LAYER)) {
        x = slow_divmod16(&m->x, KEYBALL_SLOW_CPI_DIV);
        y = slow_divmod16(&m->y, KEYBALL_SLOW_CPI_DIV);
    } else {
        x    = m->x;
        y    = m->y;
        m->x = 0;
        m->y = 0;
    }
    r->x = slow_clip2int8(y);
    r->y = slow_clip2int8(x);
    if (is_left) {
        r->x = -r->x;
        r->y = -r->y;
    }
}

//////////////////////////////////////////////////////////////////////////////
// RDP対策の設定 (処理本体は process_record_user の前)

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

// Vialのカスタムキーコード。keyball.h が QK_KB_0〜QK_KB_15 を使っているので、その続き。
// vial.jsonのcustomKeycodesの17〜19番目と順番を合わせること
#define RDP_UP QK_KB_16  // 押下側の待ちを+10ms
#define RDP_DN QK_KB_17  // 押下側の待ちを-10ms
#define RDP_TOG QK_KB_18 // 対策のオン/オフ(オフで待ち0ms、QMK標準の動作)

// EEPROM(ユーザー領域)に保存する設定。Keyballの設定はkb領域なので重ならない
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

static void rdp_config_reset(void) {
    rdp_config.raw     = 0;
    rdp_config.magic   = RDP_CONFIG_MAGIC;
    rdp_config.delay   = RDP_PRESS_DELAY;
    rdp_config.enabled = 0; // 初期状態はRDP対策オフ
    eeconfig_update_user(rdp_config.raw);
}

//////////////////////////////////////////////////////////////////////////////
// オートマウスレイヤー (AML)

// 書き込み直後やBootmagicで設定が初期化されたときは AML を無効にしておく (AML_TOで有効にすると即EEPROMへ保存)
void eeconfig_init_user(void) {
    keyball_config_t c = {.raw = 0};
    c.amle             = 0;
    eeconfig_update_kb(c.raw);
    rdp_config_reset();
}

static void rdp_sync_handler(uint8_t in_buflen, const void *in_data, uint8_t out_buflen, void *out_data) {
    if (in_buflen == 1) rdp_config.enabled = *(const uint8_t *)in_data ? 1 : 0;
}

void keyboard_post_init_user(void) {
    // AMLのタイムアウトは AUTO_MOUSE_TIME に固定 (keyball.c がEEPROMから読んだ値を上書き)
    set_auto_mouse_timeout(AUTO_MOUSE_TIME);

    // RDP設定を読み込む。初めての書き込みや範囲外の値なら初期値に戻す
    // (eeconfig_init_userはAMLの設定も初期化するので、ここではRDP設定だけを戻す)
    rdp_config.raw = eeconfig_read_user();
    if (rdp_config.magic != RDP_CONFIG_MAGIC || rdp_config.delay > RDP_DELAY_MAX || rdp_config.enabled > 1) {
        rdp_config_reset();
    }

    // USB非接続側: マスターから届いたRDPのオン/オフを表示LED用に反映 (EEPROMには保存しない)
    transaction_register_rpc(KEYBALL_RDP_SYNC, rdp_sync_handler);
}

static const uint8_t aml_keep_positions[][2] = KEYBALL_AML_KEEP_POSITIONS;
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

// マウスボタン入力後、KEYBALL_AML_CLICK_EXIT_TIME経過でdefault layerへ戻すためのタイマー
static bool     aml_click_exit_pending = false;
static uint16_t aml_click_exit_timer   = 0;

// AML解除用に握りつぶした(press/releaseとも処理しない)キーの位置
static matrix_row_t aml_swallowed_keys[MATRIX_ROWS];

bool pre_process_record_user(uint16_t keycode, keyrecord_t *record) {
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
    return true;
}

// ---------------------------------------------------------------------------
// RDP対策
// RDP上では、修飾キーとキー本体のレポートが短い間隔で続くと、修飾キーより先に
// キーが処理されて修飾が効かないことがある。修飾キーの状態が変わった直後のキー押下だけ、
// 一定時間空くまで待ってから送る。
//   - 修飾付きキーコード(KC_EXLM, LSFT(kc), LCTL(kc)など): 修飾→待ち→キー
//   - HRM(mod-tap)のホールド、物理的な修飾キー、LMなど: get_mods()の変化を検出し、
//     直後に押されたキーを待たせる(押されるのが遅ければ待ちは発生しない)
// 定数と設定(rdp_config)はAMLの前の「RDP対策の設定」で定義
// ---------------------------------------------------------------------------
static uint32_t rdp_popup_time;
static bool     rdp_popup_active;

static uint32_t rdp_mod_press_time;   // 最後に修飾キーが押された時刻
static uint32_t rdp_mod_release_time; // 最後に修飾キーが離された時刻
static uint32_t rdp_key_press_time;   // 最後にキーが押された時刻
static uint8_t  rdp_mods_before;      // 処理前の修飾キー状態(post_process_record_userで比較)

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

// 修飾キーそのものの押下か(この場合は待たない)
static bool rdp_is_mod_press(uint16_t keycode, keyrecord_t *record) {
    if (IS_MODIFIER_KEYCODE(keycode)) return true;
#ifndef NO_ACTION_TAPPING
    if (IS_QK_MOD_TAP(keycode) && record->tap.count == 0) return true; // HRMのホールド
#endif
    return false;
}

// Mod-Tap のタップ側で Shift 付き記号を出す（OSはJIS配列前提）。
// Mod-Tap はタップ側に基本キーコードしか持てず、LALT_T(S(KC_6)) と書いても Shift が落ちるため、
// タップ時の処理を横取りして Shift+キー を送る。RDP対策がオンなら同じ待ちを入れて Shift 抜けを防ぐ。
static void rdp_tap_shifted(uint8_t kc) {
    register_weak_mods(MOD_BIT(KC_LSFT));
    if (rdp_config.enabled) {
        rdp_mod_press_time = timer_read32();
        rdp_wait_after_mods(); // = 設定中の押下側の待ち
    }
    tap_code(kc);
    if (rdp_config.enabled) wait_ms(RDP_RELEASE_DELAY);
    unregister_weak_mods(MOD_BIT(KC_LSFT));
    if (rdp_config.enabled) rdp_mod_release_time = timer_read32();
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (keycode == RDP_UP || keycode == RDP_DN || keycode == RDP_TOG) {
        if (record->event.pressed) rdp_change_setting(keycode);
        return false;
    }

    rdp_mods_before = get_mods();

    if (rdp_config.enabled) { // オフ: QMK標準の動作
        if (record->event.pressed) {
            if (!rdp_is_mod_press(keycode, record)) {
                rdp_wait_after_mods();
            }
        } else {
            rdp_wait_before_release();
        }

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
    }

    // Mod-Tap のタップで Shift 付き記号を出す(RDP対策のオン/オフに関わらず有効)
    switch (keycode) {
        case LALT_T(KC_6):  // ホールド:Alt   / タップ:＆（Shift+6）
        case LCTL_T(KC_8):  // ホールド:Ctrl  / タップ:（（Shift+8）
        case LSFT_T(KC_9):  // ホールド:Shift / タップ:）（Shift+9）
            if (record->tap.count) {
                if (record->event.pressed) rdp_tap_shifted(QK_MOD_TAP_GET_TAP_KEYCODE(keycode));
                return false;  // タップの解放も横取り（基本キーの解放を送らない）
            }
            break;  // ホールドは通常の修飾キーとして処理
    }

    // マウスボタンを押したらKEYBALL_AML_CLICK_EXIT_TIME後にdefault layerへ戻す。
    // 押すたびにタイマーを再スタートするので、ダブルクリックの2打目もAMLレイヤー上で入力される。
    if (record->event.pressed && keycode >= KC_MS_BTN1 && keycode <= KC_MS_BTN8 && is_aml_layer_active()) {
        aml_click_exit_pending = true;
        aml_click_exit_timer   = timer_read();
    }

    switch (keycode) {
        case AML_TO:
            // 切り替えたら即EEPROMへ保存する
            if (record->event.pressed) {
                bool             enable = !get_auto_mouse_enable();
                keyball_config_t c      = {.raw = eeconfig_read_kb()};
                set_auto_mouse_enable(enable);
                c.amle = enable;
                eeconfig_update_kb(c.raw);
            }
            return false;
        case AML_I50:
        case AML_D50:
            // タイムアウトは AUTO_MOUSE_TIME に固定のため無効
            return false;
    }
    return true;
}

void post_process_record_user(uint16_t keycode, keyrecord_t *record) {
    // RDP対策: キー処理の後で修飾キーの状態を比較し、押された/離された時刻を記録する。
    // mod-tapのホールド(Hold On Other Key Pressで処理中にホールドへ変わる場合を含む)、
    // 物理的な修飾キー、LM(layer, mod)などをまとめて検出できる。
    uint8_t mods_after = get_mods();
    if (mods_after & ~rdp_mods_before) rdp_mod_press_time = timer_read32();
    if (rdp_mods_before & ~mods_after) rdp_mod_release_time = timer_read32();

    // KBC_RST は AML を無効に戻す (こちらの既定と同じ)。タイムアウトだけ AUTO_MOUSE_TIME に揃える
    if (keycode == KBC_RST && record->event.pressed) {
        set_auto_mouse_timeout(AUTO_MOUSE_TIME);
    }
}

//////////////////////////////////////////////////////////////////////////////
// AML表示用LED

#ifdef RGBLIGHT_ENABLE
#    ifdef KEYBALL_AML_LED_DEBUG
static uint8_t aml_led_debug_index = 0;
#    endif

// PCがスリープ(USBサスペンド)中か。AML/RDP表示LEDも消す
static bool led_suspended = false;

// AML中は青(明るさはRGBLIGHTの設定値)、それ以外は消灯
static uint8_t aml_led_val(void) {
    return !led_suspended && layer_state_is(get_auto_mouse_layer()) ? rgblight_get_val() : 0;
}

// RDP対策がオンの間は白(明るさはRGBLIGHTの設定値)、オフなら消灯
// rdp_configはUSB接続側(マスター)で更新され、USB非接続側へは housekeeping_task_user で送る
static uint8_t rdp_led_val(void) {
    return !led_suspended && rdp_config.enabled ? rgblight_get_val() : 0;
}

// スリープに入るとき: この後のrgblight_suspend()(RGBLIGHT_SLEEP)が全消灯し、そのflushで表示LEDも消える
void suspend_power_down_user(void) {
    led_suspended = true;
}

// 復帰したとき: rgblight_wakeup()で元の点灯状態に戻った後なので、表示LEDを書き直す
void suspend_wakeup_init_user(void) {
    led_suspended = false;
    rgblight_set();
}

// RGBLIGHTのドライバー (rules.mk で RGBLIGHT_DRIVER = custom)。中身は ws2812 のままで、
// LEDへ送り出す直前 (flush) に KEYBALL_AML_LED_INDEX / KEYBALL_RDP_LED_INDEX だけ差し替える。
// Vialの lighting "All Off" でも rgblight_set() は flush を呼ぶので、消灯中もAML表示は点灯する。
static void aml_led_flush(void) {
#    ifdef KEYBALL_AML_LED_DEBUG
    // 診断用: 左右それぞれのLEDを2秒ごとに1つずつ青で点灯させる (番号はUSB接続側のOLEDに表示)。
    // AMLのオン/オフに関係なく常に動く
    uint8_t num_leds    = rgblight_ranges.clipping_num_leds;
    aml_led_debug_index = (timer_read32() / 2000) % num_leds;
    for (uint8_t i = 0; i < num_leds; i++) {
        ws2812_set_color(i, 0, 0, i == aml_led_debug_index ? 100 : 0);
    }
    ws2812_flush();
    return;
#    endif
    if (is_keyboard_left() == KEYBALL_AML_LED_ON_LEFT && KEYBALL_AML_LED_INDEX < rgblight_ranges.clipping_num_leds) {
        ws2812_set_color(KEYBALL_AML_LED_INDEX, 0, 0, aml_led_val());
    }
    if (is_keyboard_left() == KEYBALL_RDP_LED_ON_LEFT && KEYBALL_RDP_LED_INDEX < rgblight_ranges.clipping_num_leds) {
        uint8_t v = rdp_led_val();
        ws2812_set_color(KEYBALL_RDP_LED_INDEX, v, v, v);
    }
    ws2812_flush();
}

const rgblight_driver_t rgblight_driver = {
    .init          = ws2812_init,
    .set_color     = ws2812_set_color,
    .set_color_all = ws2812_set_color_all,
    .flush         = aml_led_flush,
};
#endif

void housekeeping_task_user(void) {
    if (aml_click_exit_pending && timer_elapsed(aml_click_exit_timer) > KEYBALL_AML_CLICK_EXIT_TIME) {
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

#ifdef RGBLIGHT_ENABLE
    // AML/RDP状態が変わったらLEDを書き直す (静止パターンやAll Off中はRGBLIGHTが再描画しないため)。
    // All Off中は、切り替え直後の全消灯などで上書きされる場合に備えて定期的にも書き直す
    static uint16_t refresh_timer = 0;
#    ifdef KEYBALL_AML_LED_DEBUG
    // 診断用: 左右とも定期的に書き直して点灯位置を進める
    if (timer_elapsed(refresh_timer) > 100) {
        rgblight_set();
        refresh_timer = timer_read();
    }
#    else
    // マスター: RDPのオン/オフを変化時と500msごとにUSB非接続側へ送る
    static uint8_t  last_rdp_sent  = 0xFF;
    static uint16_t rdp_sync_timer = 0;
    if (is_keyboard_master() && (rdp_config.enabled != last_rdp_sent || timer_elapsed(rdp_sync_timer) > 500)) {
        // 送れなかった場合 (片手だけ接続中など) も、500msごとの再送で追いつく
        last_rdp_sent = rdp_config.enabled;
        transaction_rpc_send(KEYBALL_RDP_SYNC, sizeof(last_rdp_sent), &last_rdp_sent);
        rdp_sync_timer = timer_read();
    }

    static int32_t last_val = -1;
    int32_t        aml_val  = 0;
    if (is_keyboard_left() == KEYBALL_AML_LED_ON_LEFT) aml_val |= aml_led_val();
    if (is_keyboard_left() == KEYBALL_RDP_LED_ON_LEFT) aml_val |= (int32_t)rdp_led_val() << 8;
    if (aml_val != last_val || (!rgblight_is_enabled() && timer_elapsed(refresh_timer) > 100)) {
        rgblight_set();
        last_val      = aml_val;
        refresh_timer = timer_read();
    }
#    endif
#endif
}

#ifdef OLED_ENABLE

#    include "lib/oledkit/oledkit.h"

#    define KEYBALL_OLED_COLS 21 // 128px / 6px(フォント幅)

// 4行目のレイヤー情報。keyball_oled_render_layerinfo() と同じ形式で、末尾のAMLタイムアウト
// (AUTO_MOUSE_TIMEに固定)の代わりにRDP対策のオン'R'/オフ'-'を表示する。
// RDP_UP/RDP_DN/RDP_TOGを押してからRDP_POPUP_TIMEの間だけ、設定値の表示に切り替える。
static __attribute__((unused)) void render_layer_rdp_info(void) { // 診断用ファームでは未使用
    char    line[KEYBALL_OLED_COLS + 1];
    uint8_t n = 0;

    if (rdp_popup_active && timer_elapsed32(rdp_popup_time) >= RDP_POPUP_TIME) {
        rdp_popup_active = false;
    }
    if (rdp_popup_active) {
        if (rdp_config.enabled) {
            n = snprintf(line, sizeof(line), "RDP ON   %3ums", (unsigned)rdp_config.delay);
        } else {
            n = snprintf(line, sizeof(line), "RDP OFF  (%3ums)", (unsigned)rdp_config.delay);
        }
    } else {
        memcpy(line, "L\xB6\xB7r\xB1", 5); // "Layer:"
        n = 5;
        for (uint8_t i = 1; i < 8; i++) {
            line[n++] = layer_state_is(i) ? '0' + i : '\xB0';
        }
        line[n++] = ' ';
        line[n++] = '\xC2'; // "AML"
        line[n++] = '\xC3';
        line[n++] = get_auto_mouse_enable() ? '\xB2' : '\xB4'; // "ON" / "OFF"
        line[n++] = get_auto_mouse_enable() ? '\xB3' : '\xB5';
        line[n++] = ' ';
        line[n++] = rdp_config.enabled ? 'R' : '-';
    }

    // 1行ちょうどの文字数にそろえる(前の表示の消え残りと、次の行の表示位置のずれを防ぐ)
    while (n < KEYBALL_OLED_COLS) line[n++] = ' ';
    line[KEYBALL_OLED_COLS] = '\0';
    oled_write(line, false);
}

void oledkit_render_info_user(void) {
    keyball_oled_render_keyinfo();
    keyball_oled_render_ballinfo();
#    if defined(KEYBALL_AML_LED_DEBUG) && defined(RGBLIGHT_ENABLE)
    // 診断用: 点灯中のLED番号 (0始まり) を表示
    // 診断用: USB接続側の手と、点灯中のLED番号 (0始まり) を表示
    oled_write_P(is_keyboard_left() ? PSTR("LEFT  LED index: ") : PSTR("RIGHT LED index: "), false);
    oled_write_char('0' + aml_led_debug_index / 10, false);
    oled_write_char('0' + aml_led_debug_index % 10, false);
    oled_write_P(PSTR("  "), false);
#    else
    render_layer_rdp_info();
#    endif
}
#endif
