# Pro Micro 互換の RP2040 ボード (SparkFun Pro Micro RP2040 と同じピン配置) 向けに変換する
CONVERT_TO = sparkfun_pm2040

RGBLIGHT_ENABLE = yes
# AML表示用LEDを差し替えるため、ws2812 を包んだ独自ドライバー (keymap.c の rgblight_driver) を使う
RGBLIGHT_DRIVER = custom
WS2812_DRIVER_REQUIRED = yes

OLED_ENABLE = yes

# 指定キーを押しながら USB 接続でブートローダーに入る
BOOTMAGIC_ENABLE = yes

VIA_ENABLE = yes
VIAL_ENABLE = yes
