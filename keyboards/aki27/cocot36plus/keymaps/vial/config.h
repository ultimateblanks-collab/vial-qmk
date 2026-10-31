
#define VIAL_KEYBOARD_UID {0xC8, 0x4D, 0xC8, 0xD5, 0xD8, 0x28, 0x66, 0x16}
#define VIAL_UNLOCK_COMBO_ROWS {0, 0}
#define VIAL_UNLOCK_COMBO_COLS {1, 9}

#define POINTING_DEVICE_AUTO_MOUSE_ENABLE
#define AUTO_MOUSE_DEFAULT_LAYER 1
#define AUTO_MOUSE_TIME 30000   // AMLのタイムアウト(ms) uint16_tのため最大65535
#define AUTO_MOUSE_DELAY 300   // キー入力から300ms間はAMLを発動しない
#define AUTO_MOUSE_DEBOUNCE 25
#define COCOT_AML_CLICK_EXIT_TIME 250   // マウスボタン入力から250ms後にdefault layerへ戻る

#define RGB_MATRIX_SLEEP         // PCのスリープ中はRGB Matrixを全消灯(AML/RDP表示用LEDもここで消える)

#define COCOT_CPI_DEFAULT 0      // EEPROM初期化時のCPI: COCOT_CPI_OPTIONSの0番目(500)

#define COCOT_SLOW_CPI_LAYER 5   // このレイヤーが有効な間はCPIを落とす
#define COCOT_SLOW_CPI_DIV 5     // 設定値の1/5

// AML中に押してもAMLを維持するキーポジション {row, col}
// これ以外のポジションが押されると即default layerへ戻り、そのキーはdefault layer側のキーとして入力される
// matrixの位置は v1/info.json の "matrix" を参照
//   {1, 0}: LGUI_T(A), {1, 1}〜{1, 4}: S D F G, {1, 9}: TD(2)
//   {3, 1}〜{3, 3}: 左親指 LT(5,ESC) LT(2,ENT) LT(7,DEL) (default layerでの位置)
#define COCOT_AML_KEEP_POSITIONS { {1, 0}, {1, 1}, {1, 2}, {1, 3}, {1, 4}, {1, 9}, {3, 1}, {3, 2}, {3, 3} }
