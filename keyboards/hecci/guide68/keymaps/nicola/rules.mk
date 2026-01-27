# MCU name
MCU = atmega32u4

# Bootloader selection
BOOTLOADER = caterina

# Build Options
#   change yes to no to disable
LTO_ENABLE = yes		# Save Memory
CONSOLE_ENABLE = no	# Console for debug

SRC += nicola.c \
	jtu.c \
 	key_duration.c\
	a2j/translate_ansi_to_jis.c

# 16-bit timer (TIMER1) を使用したタイムアウト
# 一定時間経過すると、key down 状態のままでもキーが出力される
OPT_DEFS = -DTIMEOUT_INTERRUPT

# NICOLA key down timer uses TIMER1 so Backlight LED must be turned off
BACKLIGHT_ENABLE = no       # Enable keyboard backlight functionality
