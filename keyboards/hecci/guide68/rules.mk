# MCU name
MCU = atmega32u4

# Bootloader selection
BOOTLOADER = caterina

# Build Options
#   change yes to no to disable
#
LTO_ENABLE = yes
CONSOLE_ENABLE = no          # Console for debug

SRC += a2j/translate_ansi_to_jis.c

