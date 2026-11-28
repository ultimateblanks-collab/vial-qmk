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
    RGB_MOD  , RGB_HUI  , RGB_SAI  , RGB_VAI  , SCRL_DVI ,                            CW_TOGG  , _______  , _______  , _______  , _______  ,
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
#define SSNP_TOG QK_KB_19 // スクロールスナップ: VERTICAL <-> HORIZONTAL をトグル

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

// USB非接続側で、マスターから届いたCaps Wordの状態
static bool caps_word_remote = false;

// 同期データ: bit0 = RDP対策オン, bit1 = Caps Wordオン
static void rdp_sync_handler(uint8_t in_buflen, const void *in_data, uint8_t out_buflen, void *out_data) {
    if (in_buflen == 1) {
        uint8_t v          = *(const uint8_t *)in_data;
        rdp_config.enabled = (v & 1) ? 1 : 0;
        caps_word_remote   = (v & 2) ? true : false;
    }
}

#ifdef OLED_ENABLE
// デフォルト以外のレイヤー中にOLEDへ出す、自分側のキーマップの同期 (実装はファイル後半のOLED節)
static void oled_keymap_sync_handler(uint8_t in_buflen, const void *in_data, uint8_t out_buflen, void *out_data);
static void oled_keymap_sync_task(void);
#endif

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
#ifdef OLED_ENABLE
    // USB非接続側: マスターから届いた自分側のキーマップをOLED表示用に受け取る
    transaction_register_rpc(KEYBALL_KEYMAP_SYNC, oled_keymap_sync_handler);
#endif
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

    if (keycode == SSNP_TOG) {
        if (record->event.pressed) {
            keyball_set_scrollsnap_mode(
                keyball_get_scrollsnap_mode() == KEYBALL_SCROLLSNAP_MODE_VERTICAL
                    ? KEYBALL_SCROLLSNAP_MODE_HORIZONTAL
                    : KEYBALL_SCROLLSNAP_MODE_VERTICAL);
        }
        return false;
    }
    // Caps Word中の「-」は、JIS配列で「_」(Shift+KC_INT1)を入力してCaps Wordを継続する。
    // QMK標準ではKC_MINSにShiftが付き、JIS配列では「=」になってしまうため。
    // 「-」がVialのタップダンスに割り当てられている場合(タップ動作がKC_MINS)も同様に扱う。
    // この場合、Caps Word中はタップダンスのホールド/ダブルタップ等は無効になる。
    // 解放側も横取りして基本キーの解放を送らない。
    {
        bool is_minus = (keycode == KC_MINS);
#ifdef VIAL_TAP_DANCE_ENABLE
        if (IS_QK_TAP_DANCE(keycode)) {
            vial_tap_dance_entry_t td;
            if (dynamic_keymap_get_tap_dance(QK_TAP_DANCE_GET_INDEX(keycode), &td) == 0 && td.on_tap == KC_MINS) is_minus = true;
        }
#endif
        if (is_minus && is_caps_word_on() && !(get_mods() & ~MOD_BIT(KC_LSFT) & ~MOD_BIT(KC_RSFT))) {
            if (record->event.pressed) {
                rdp_tap_shifted(KC_INT1);
#if CAPS_WORD_IDLE_TIMEOUT > 0
                caps_word_reset_idle_timer();
#endif
            }
            return false;
        }
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

// アンダーグローの表示状態: 0=通常, 1=RDP対策オン(白), 2=Caps Wordオン(青。両方ならこちら優先), 3=レイヤー6(右手のみ緑。最優先)
// rdp_configとCaps WordはUSB接続側(マスター)で更新され、USB非接続側へは housekeeping_task_user で送る
static uint8_t ug_state(void) {
    if (led_suspended) return 0;
    if (!is_keyboard_left() && layer_state_is(6)) return 3; // 右手のみ・レイヤー6で緑
    bool caps = is_keyboard_master() ? is_caps_word_on() : caps_word_remote;
    if (caps) return 2;
    return rdp_config.enabled ? 1 : 0;
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
// LEDへ送り出す直前 (flush) に KEYBALL_AML_LED_INDEX とアンダーグロー(KEYBALL_UG_*)だけ差し替える。
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
    // アンダーグロー: RDP対策オンで白、Caps Wordオンで青 (どちらでもなければRGBLIGHT本来の色のまま)
    uint8_t ug = ug_state();
    if (ug) {
        uint8_t start = is_keyboard_left() ? KEYBALL_UG_LEFT_START : KEYBALL_UG_RIGHT_START;
        uint8_t count = is_keyboard_left() ? KEYBALL_UG_LEFT_COUNT : KEYBALL_UG_RIGHT_COUNT;
        uint8_t v     = rgblight_get_val();
        for (uint8_t i = start; i < start + count && i < rgblight_ranges.clipping_num_leds; i++) {
            if (ug == 3) {
                ws2812_set_color(i, 0, v, 0);
            } else if (ug == 2) {
                ws2812_set_color(i, 0, 0, v);
            } else {
                ws2812_set_color(i, v, v, v);
            }
        }
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
#ifdef OLED_ENABLE
    oled_keymap_sync_task();
#endif
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
    // マスター: RDPとCaps Wordのオン/オフを変化時と500msごとにUSB非接続側へ送る
    static uint8_t  last_sync_sent = 0xFF;
    static uint16_t rdp_sync_timer = 0;
    uint8_t         sync_now       = (rdp_config.enabled ? 1 : 0) | (is_caps_word_on() ? 2 : 0);
    if (is_keyboard_master() && (sync_now != last_sync_sent || timer_elapsed(rdp_sync_timer) > 500)) {
        // 送れなかった場合 (片手だけ接続中など) も、500msごとの再送で追いつく
        last_sync_sent = sync_now;
        transaction_rpc_send(KEYBALL_RDP_SYNC, sizeof(sync_now), &sync_now);
        rdp_sync_timer = timer_read();
    }

    static int32_t last_val = -1;
    int32_t        aml_val  = 0;
    if (is_keyboard_left() == KEYBALL_AML_LED_ON_LEFT) aml_val |= aml_led_val();
    aml_val |= (int32_t)ug_state() << 8;
    aml_val |= (int32_t)rgblight_get_val() << 16; // 明るさを変えたときも書き直す
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

//////////////////////////////////////////////////////////////////////////////
// デフォルト以外のレイヤー中、各OLEDに「その手側のキーマップ」を表示する
//
// 表示は4行。1〜3行目が各段5キー(1キー4桁: 3文字+空白)、4行目が親指段の6キー(1キー3桁: 2文字+空白)。
// 1行目の右端には現在のレイヤー番号を出す。KC_TRNSとKC_NOは空欄。
// 日本語(JIS)配列のVialキーマップ向けに、記号は JIS での入力結果で表示する。
//
// Vialのキーマップ(dynamic keymap)はUSB接続側(マスター)のEEPROMにしか入っていないので、
// 非接続側の分はマスターが読み出してRPCで送る (1回のRPCが32バイトに収まるよう2つに分割)。

#    include "dynamic_keymap.h"

#    define OKM_KEYS 21        // 片側のキー数 (5キー x 3段 + 親指6キー)
#    define OKM_MAIN_KEYS 15   // 5キー x 3段
#    define OKM_CHUNK0_KEYS 11 // RPC 1回目に送るキー数 (残りは2回目)
#    define OKM_SYNC_INTERVAL 500

// 1キー分を w 文字ちょうどで o に書く。長い場合は切り詰める。
// w=2 のときは "MO1" -> "M1" のように、末尾が数字の3文字は先頭と数字を残す
static void okm_put(char *o, uint8_t w, const char *s) {
    char    c[4] = {0};
    uint8_t len  = strlen(s);
    if (w == 2 && len == 3 && s[2] >= '0' && s[2] <= '9' && !(s[1] >= '0' && s[1] <= '9')) {
        c[0] = s[0];
        c[1] = s[2];
        s    = c;
        len  = 2;
    }
    uint8_t i = 0;
    for (; i < w && i < len; i++) o[i] = s[i];
    for (; i < w; i++) o[i] = ' ';
}

// Shift を押したときの JIS 配列での記号。該当なしは NULL
static const char *okm_shift_symbol(uint8_t kc) {
    switch (kc) {
        case KC_1: return "!";
        case KC_2: return "\"";
        case KC_3: return "#";
        case KC_4: return "$";
        case KC_5: return "%";
        case KC_6: return "&";
        case KC_7: return "'";
        case KC_8: return "(";
        case KC_9: return ")";
        case KC_MINS: return "=";
        case KC_EQL: return "~";
        case KC_LBRC: return "`";
        case KC_RBRC: return "{";
        case KC_BSLS: return "}";
        case KC_NUHS: return "}";
        case KC_SCLN: return "+";
        case KC_QUOT: return "*";
        case KC_COMM: return "<";
        case KC_DOT: return ">";
        case KC_SLSH: return "?";
        case KC_INT1: return "_";
        case KC_INT3: return "|";
        default: return NULL;
    }
}

// 基本キーコードの表示名 (最大3文字)。tmp は8バイト以上。未対応は NULL
static const char *okm_basic_name(uint8_t kc, char *tmp) {
    if (kc >= KC_A && kc <= KC_Z) {
        tmp[0] = 'a' + (kc - KC_A);
        tmp[1] = '\0';
        return tmp;
    }
    if (kc >= KC_1 && kc <= KC_9) {
        tmp[0] = '1' + (kc - KC_1);
        tmp[1] = '\0';
        return tmp;
    }
    if (kc >= KC_F1 && kc <= KC_F12) {
        snprintf(tmp, 8, "F%u", (unsigned)(kc - KC_F1 + 1));
        return tmp;
    }
    switch (kc) {
        case KC_0: return "0";
        case KC_ENT: return "Ent";
        case KC_ESC: return "Esc";
        case KC_BSPC: return "Bsp";
        case KC_TAB: return "Tab";
        case KC_SPC: return "Spc";
        case KC_MINS: return "-";
        case KC_EQL: return "^";
        case KC_LBRC: return "@";
        case KC_RBRC: return "[";
        case KC_BSLS: return "]";
        case KC_NUHS: return "]";
        case KC_SCLN: return ";";
        case KC_QUOT: return ":";
        case KC_GRV: return "Zen";
        case KC_COMM: return ",";
        case KC_DOT: return ".";
        case KC_SLSH: return "/";
        case KC_INT1: return "\\";
        case KC_INT2: return "Kat";
        case KC_INT3: return "Yen";
        case KC_INT4: return "Hen";
        case KC_INT5: return "Muh";
        case KC_LNG1: return "Ln1";
        case KC_LNG2: return "Ln2";
        case KC_CAPS: return "Cap";
        case KC_PSCR: return "Prt";
        case KC_SCRL: return "Scr";
        case KC_PAUS: return "Pau";
        case KC_INS: return "Ins";
        case KC_HOME: return "Hom";
        case KC_PGUP: return "PgU";
        case KC_DEL: return "Del";
        case KC_END: return "End";
        case KC_PGDN: return "PgD";
        case KC_RGHT: return "Rgt";
        case KC_LEFT: return "Lft";
        case KC_DOWN: return "Dwn";
        case KC_UP: return "Up";
        case KC_LCTL: return "Ctl";
        case KC_LSFT: return "Sft";
        case KC_LALT: return "Alt";
        case KC_LGUI: return "Gui";
        case KC_RCTL: return "Ctl";
        case KC_RSFT: return "Sft";
        case KC_RALT: return "Alt";
        case KC_RGUI: return "Gui";
        case KC_BTN1: return "MB1";
        case KC_BTN2: return "MB2";
        case KC_BTN3: return "MB3";
        case KC_BTN4: return "MB4";
        
        default: return NULL;
    }
}

// キーコードを w 文字(2か3)のラベルにして o に書く。depth は LT/MT/タップダンスをたどる深さの制限
static void okm_label(uint16_t kc, uint8_t w, char *o, uint8_t depth) {
    char        t[8];
    char        t2[8];
    const char *s = NULL;

    if (kc == KC_NO || kc == KC_TRNS) {
        s = "";
    } else if (IS_QK_BASIC(kc)) {
        s = okm_basic_name((uint8_t)kc, t);
    } else if (IS_QK_MODS(kc)) {
        uint8_t mods = QK_MODS_GET_MODS(kc);
        uint8_t base = QK_MODS_GET_BASIC_KEYCODE(kc);
        if ((mods & 0x0F) == 0x02) { // Shift のみ
            if (base >= KC_A && base <= KC_Z) {
                t[0] = 'A' + (base - KC_A);
                t[1] = '\0';
                s    = t;
            } else {
                s = okm_shift_symbol(base);
            }
        }
        if (!s) { // その他の修飾の組み合わせ: 修飾の頭文字 + 元のキー
            const char *bn = okm_basic_name(base, t2);
            char        p  = (mods & 0x01) ? 'C' : (mods & 0x02) ? 'S' : (mods & 0x04) ? 'A' : 'G';
            snprintf(t, sizeof(t), "%c%s", p, bn ? bn : "?");
            s = t;
        }
    } else if (IS_QK_LAYER_TAP(kc)) {
        if (depth < 2) {
            okm_label(QK_LAYER_TAP_GET_TAP_KEYCODE(kc), w, o, depth + 1);
            return;
        }
    } else if (IS_QK_MOD_TAP(kc)) {
        if (depth < 2) {
            okm_label(QK_MOD_TAP_GET_TAP_KEYCODE(kc), w, o, depth + 1);
            return;
        }
    } else if (IS_QK_MOMENTARY(kc)) {
        snprintf(t, sizeof(t), "MO%u", (unsigned)QK_MOMENTARY_GET_LAYER(kc));
        s = t;
    } else if (IS_QK_TO(kc)) {
        snprintf(t, sizeof(t), "TO%u", (unsigned)QK_TO_GET_LAYER(kc));
        s = t;
    } else if (IS_QK_TOGGLE_LAYER(kc)) {
        snprintf(t, sizeof(t), "Tg%u", (unsigned)QK_TOGGLE_LAYER_GET_LAYER(kc));
        s = t;
    } else if (IS_QK_DEF_LAYER(kc)) {
        snprintf(t, sizeof(t), "DF%u", (unsigned)QK_DEF_LAYER_GET_LAYER(kc));
        s = t;
    } else if (IS_QK_ONE_SHOT_LAYER(kc)) {
        snprintf(t, sizeof(t), "OS%u", (unsigned)(kc & 0x1F));
        s = t;
    } else if (IS_QK_LAYER_TAP_TOGGLE(kc)) {
        snprintf(t, sizeof(t), "tt%u", (unsigned)QK_LAYER_TAP_TOGGLE_GET_LAYER(kc));
        s = t;
    } else if (IS_QK_TAP_DANCE(kc)) {
        // 1回タップしたときのキーを表示 (Vialのタップダンス設定から読む)
        vial_tap_dance_entry_t td;
        uint8_t                idx = QK_TAP_DANCE_GET_INDEX(kc);
        if (depth < 2 && dynamic_keymap_get_tap_dance(idx, &td) == 0 && td.on_tap != KC_NO) {
            okm_label(td.on_tap, w, o, depth + 1);
            return;
        }
        snprintf(t, sizeof(t), "TD%u", (unsigned)idx);
        s = t;
    } else if (IS_QK_MACRO(kc)) {
        unsigned n = kc - QK_MACRO;
        if (n < 100) {
            snprintf(t, sizeof(t), "m%u", n);
            s = t;
        } else {
            s = "m**";
        }
    } else {
        switch (kc) {
            case QK_BOOT: s = "Bot"; break;
            case CW_TOGG: s = "CW"; break;
            case RGB_TOG: s = "RGB"; break;
            case RGB_MOD: s = "Rm+"; break;
            case RGB_RMOD: s = "Rm-"; break;
            case RGB_HUI: s = "H+"; break;
            case RGB_HUD: s = "H-"; break;
            case RGB_SAI: s = "S+"; break;
            case RGB_SAD: s = "S-"; break;
            case RGB_VAI: s = "V+"; break;
            case RGB_VAD: s = "V-"; break;
            case KBC_RST: s = "Rst"; break;
            case KBC_SAVE: s = "Sav"; break;
            case CPI_I100: s = "C+1"; break;
            case CPI_D100: s = "C-1"; break;
            case CPI_I1K: s = "C+K"; break;
            case CPI_D1K: s = "C-K"; break;
            case SCRL_TO: s = "ScT"; break;
            case SCRL_MO: s = "ScM"; break;
            case SCRL_DVI: s = "Sd+"; break;
            case SCRL_DVD: s = "Sd-"; break;
            case SSNP_VRT: s = "SVT"; break;
            case SSNP_HOR: s = "SHO"; break;
            case SSNP_FRE: s = "SFR"; break;
            case AML_TO: s = "AML"; break;
            case AML_I50: s = "A+5"; break;
            case AML_D50: s = "A-5"; break;
            default: break;
        }
    }
    okm_put(o, w, s ? s : "***");
}

// layer の、left 側(true:左手, false:右手)のキーコードを OKM_KEYS 個読む。
// 並びは見た目どおり: 0〜14 が上3段(左から右へ5キーずつ)、15〜20 が親指段(左から右へ6キー)。
// 左手はマトリクスの行0〜3、右手は行4〜7で、右手は列が右から左の順に並ぶ
static void okm_fetch(uint8_t layer, bool left, uint16_t *kc) {
    for (uint8_t i = 0; i < OKM_KEYS; i++) {
        uint8_t row, col;
        if (i < OKM_MAIN_KEYS) {
            row = (left ? 0 : 4) + i / 5;
            col = left ? (i % 5) : (4 - i % 5);
        } else {
            uint8_t t = i - OKM_MAIN_KEYS;
            row       = left ? 3 : 7;
            col       = left ? t : (5 - t);
        }
        kc[i] = (layer < DYNAMIC_KEYMAP_LAYER_COUNT) ? dynamic_keymap_get_keycode(layer, row, col) : KC_NO;
    }
}

// 4行(各21桁ちょうど)を書く。ちょうど21桁書くので改行は自動で、前の表示の消え残りも出ない
static void okm_render(const uint16_t *kc, uint8_t layer) {
    char line[KEYBALL_OLED_COLS + 1];

    for (uint8_t r = 0; r < 3; r++) {
        memset(line, ' ', KEYBALL_OLED_COLS);
        for (uint8_t c = 0; c < 5; c++) {
            okm_label(kc[r * 5 + c], 3, &line[c * 4], 0);
        }
        if (r == 0) line[KEYBALL_OLED_COLS - 1] = '0' + (layer % 10);
        line[KEYBALL_OLED_COLS] = '\0';
        oled_write(line, false);
    }

    memset(line, ' ', KEYBALL_OLED_COLS);
    for (uint8_t c = 0; c < 6; c++) {
        okm_label(kc[OKM_MAIN_KEYS + c], 2, &line[c * 3], 0);
    }
    line[KEYBALL_OLED_COLS] = '\0';
    oled_write(line, false);
}

static uint8_t okm_current_layer(void) {
    return get_highest_layer(layer_state | default_layer_state);
}

// USB非接続側: マスターから届いたキーマップ。2回に分けて届くので、両方のレイヤー番号が揃ったときだけ表示する
static uint8_t  okm_remote_layer[2];
static uint16_t okm_remote_kc[OKM_KEYS];

// 同期データ: [0]=何回目か(0/1), [1]=レイヤー番号, [2〜]=キーコード(下位, 上位の順)
static void oled_keymap_sync_handler(uint8_t in_buflen, const void *in_data, uint8_t out_buflen, void *out_data) {
    const uint8_t *d = (const uint8_t *)in_data;
    if (in_buflen < 2 || d[0] > 1) return;
    uint8_t chunk = d[0];
    uint8_t n     = chunk == 0 ? OKM_CHUNK0_KEYS : OKM_KEYS - OKM_CHUNK0_KEYS;
    uint8_t start = chunk == 0 ? 0 : OKM_CHUNK0_KEYS;
    if (in_buflen < 2 + 2 * n) return;
    for (uint8_t j = 0; j < n; j++) {
        okm_remote_kc[start + j] = d[2 + 2 * j] | ((uint16_t)d[3 + 2 * j] << 8);
    }
    okm_remote_layer[chunk] = d[1];
}

// マスター: USB非接続側の手のキーマップを、レイヤーが変わったときと500msごとに送る。
// 送れなかった場合 (片手だけ接続中など) も、定期的な再送で追いつく。Vialでキーマップを書き換えた場合も反映される
static void oled_keymap_sync_task(void) {
    static uint8_t  last_layer = 0xFF;
    static uint16_t timer      = 0;
    if (!is_keyboard_master()) return;

    uint8_t layer = okm_current_layer();
    if (layer == last_layer && timer_elapsed(timer) < OKM_SYNC_INTERVAL) return;
    last_layer = layer;
    timer      = timer_read();

    uint16_t kc[OKM_KEYS];
    uint8_t  buf[2 + 2 * OKM_CHUNK0_KEYS];
    okm_fetch(layer, !is_keyboard_left(), kc);
    for (uint8_t chunk = 0; chunk < 2; chunk++) {
        uint8_t n     = chunk == 0 ? OKM_CHUNK0_KEYS : OKM_KEYS - OKM_CHUNK0_KEYS;
        uint8_t start = chunk == 0 ? 0 : OKM_CHUNK0_KEYS;
        buf[0]        = chunk;
        buf[1]        = layer;
        for (uint8_t j = 0; j < n; j++) {
            buf[2 + 2 * j] = kc[start + j] & 0xFF;
            buf[3 + 2 * j] = kc[start + j] >> 8;
        }
        transaction_rpc_send(KEYBALL_KEYMAP_SYNC, 2 + 2 * n, buf);
    }
}

// USB非接続側のOLED。レイヤー0はいつものロゴ、それ以外は自分側のキーマップ
void oledkit_render_logo_user(void) {
    static bool keymap_shown = false;

    if (okm_remote_layer[0] != okm_remote_layer[1]) return; // 受信途中: 前回の表示のままにする
    if (okm_remote_layer[0] != 0) {
        okm_render(okm_remote_kc, okm_remote_layer[0]);
        keymap_shown = true;
        return;
    }
    if (keymap_shown) { // キーマップ表示から戻ったとき、4行目の消え残りを消す (ロゴは3行)
        oled_clear();
        keymap_shown = false;
    }
    // 以下は lib/oledkit/oledkit.c の既定のロゴ表示と同じ
    char ch = 0x80;
    for (int y = 0; y < 3; y++) {
        oled_write_P(PSTR("  "), false);
        for (int x = 0; x < 16; x++) {
            oled_write_char(ch++, false);
        }
        oled_advance_page(false);
    }
}

// OLEDの向き。既定 (lib/oledkit/oledkit.c) は、USB非接続側だけ180度回転。
// 左手側はそれを上下反転する (左右とも、接続の仕方によらず左手側が既定と逆向きになる)
oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    bool rotate = !is_keyboard_master();
    if (is_keyboard_left()) rotate = !rotate;
    return rotate ? OLED_ROTATION_180 : rotation;
}

void oledkit_render_info_user(void) {
    // デフォルト以外のレイヤー: USB接続側の手のキーマップを表示
    if (okm_current_layer() != 0) {
        uint16_t kc[OKM_KEYS];
        uint8_t  layer = okm_current_layer();
        okm_fetch(layer, is_keyboard_left(), kc);
        okm_render(kc, layer);
        return;
    }
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
