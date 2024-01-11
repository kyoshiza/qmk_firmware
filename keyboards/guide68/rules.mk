# MCU name
MCU = atmega32u4

# Bootloader selection
BOOTLOADER = caterina

# Build Options
#   change yes to no to disable
#

LTO_ENABLE = yes
# BOOTMAGIC_ENABLE = yes     # Enable Bootmagic Lite
MOUSEKEY_ENABLE = no       # Mouse keys
EXTRAKEY_ENABLE = no       # Audio control and System control
CONSOLE_ENABLE = no          # Console for debug
COMMAND_ENABLE = no        # Commands for debug and configuration
# NKRO_ENABLE = yes                # Enable N-Key Rollover
# BACKLIGHT_ENABLE = no       # Enable keyboard backlight functionality
# AUDIO_ENABLE = no               # Audio output
# RGBLIGHT_ENABLE = yes        # Enable WS2812 RGB underlight.
# RGBLIGHT_SLEEP = yes
# IOS_DEVICE_ENABLE = yes      # Enabe iPhone to connect the keyboard
# SPLIT_KEYBOARD = yes

SRC += a2j/translate_ansi_to_jis.c

DEFAULT_FOLDER = guide68
