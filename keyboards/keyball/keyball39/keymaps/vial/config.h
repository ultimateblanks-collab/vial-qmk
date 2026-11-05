/*
This is the c configuration file for the keymap

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

#pragma once

// Vial
#define VIAL_KEYBOARD_UID {0x48, 0x15, 0x5C, 0xD9, 0x9B, 0xD4, 0xC8, 0x49}
// Unlock combo: L00 (Q) + R00 (P)
#define VIAL_UNLOCK_COMBO_ROWS {0, 4}
#define VIAL_UNLOCK_COMBO_COLS {0, 0}

#define DYNAMIC_KEYMAP_LAYER_COUNT 6
#define DYNAMIC_KEYMAP_MACRO_COUNT 128

// Bootmagic: 左手は L00 (Q)、右手は R00 (P) を押しながら USB 接続でブートローダーへ
#define BOOTMAGIC_ROW          0
#define BOOTMAGIC_COLUMN       0
#define BOOTMAGIC_ROW_RIGHT    4
#define BOOTMAGIC_COLUMN_RIGHT 0

// AML表示用LEDの独自ドライバー (RGBLIGHT_DRIVER = custom) で ws2812 を使うため、LED数を明示する
#define WS2812_LED_COUNT RGBLIGHT_LED_COUNT

#ifdef RGBLIGHT_ENABLE
#    define RGBLIGHT_SLEEP // PCがスリープ(USBサスペンド)したらLEDを消す
#    define RGBLIGHT_EFFECT_BREATHING
#    define RGBLIGHT_EFFECT_RAINBOW_MOOD
#    define RGBLIGHT_EFFECT_RAINBOW_SWIRL
#    define RGBLIGHT_EFFECT_SNAKE
#    define RGBLIGHT_EFFECT_KNIGHT
#    define RGBLIGHT_EFFECT_CHRISTMAS
#    define RGBLIGHT_EFFECT_STATIC_GRADIENT
#    define RGBLIGHT_EFFECT_RGB_TEST
#    define RGBLIGHT_EFFECT_ALTERNATING
#    define RGBLIGHT_EFFECT_TWINKLE
#endif

#define TAP_CODE_DELAY 5

#define POINTING_DEVICE_AUTO_MOUSE_ENABLE
#define AUTO_MOUSE_DEFAULT_LAYER 1
#define AUTO_MOUSE_TIME 30000   // AMLのタイムアウト(ms) uint16_tのため最大65535。AML_I50/AML_D50は無効で、この値に固定
#define AUTO_MOUSE_DELAY 300    // キー入力から300ms間はAMLを発動しない
#define AUTO_MOUSE_DEBOUNCE 25
#define KEYBALL_AML_CLICK_EXIT_TIME 250   // マウスボタン入力から250ms後にdefault layerへ戻る

// AML中に押してもAMLを維持するキーポジション {row, col}
// これ以外のポジションが押されると即default layerへ戻り、そのキーはdefault layer側のキーとして入力される
// matrixの位置は keyball39.h の LAYOUT_no_ball を参照
//   {1, 0}〜{1, 4}: A S D F G, {5, 4}〜{5, 1}: H J K L, {5, 0}: MINS (default layerでの位置)
//   {3, 3}〜{3, 5}: 左親指3キー (L33 L34 L35)
#define KEYBALL_AML_KEEP_POSITIONS { {1, 0}, {1, 1}, {1, 2}, {1, 3}, {1, 4}, {5, 0}, {5, 1}, {5, 2}, {5, 3}, {5, 4}, {3, 3}, {3, 4}, {3, 5} }

// このレイヤーが有効な間はカーソル移動量を 1/KEYBALL_SLOW_CPI_DIV にする (CPIを落とすのと同等)
#define KEYBALL_SLOW_CPI_LAYER 5
#define KEYBALL_SLOW_CPI_DIV 5

// AML表示用LED: LED番号 (左右それぞれ0始まり) と、左手側か (true: 左手, false: 右手)
// 診断用ファーム (make ... EXTRAFLAGS=-DKEYBALL_AML_LED_DEBUG) で番号を調べられる
#define KEYBALL_AML_LED_INDEX 17
#define KEYBALL_AML_LED_ON_LEFT true

// RDP対策がオンの間に白で点灯するLED: LED番号 (左右それぞれ0始まり) と、左手側か
#define KEYBALL_RDP_LED_INDEX 9
#define KEYBALL_RDP_LED_ON_LEFT false

// RDP対策のオン/オフをUSB非接続側へ送る (RDP表示LEDがUSB非接続側にある場合のため)
#define SPLIT_TRANSACTION_IDS_USER KEYBALL_RDP_SYNC

// AML表示用LEDを右手側でも判定できるよう、レイヤー状態を左右で同期する
#define SPLIT_LAYER_STATE_ENABLE
