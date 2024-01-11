/* Copyright 2022 Salicylic_acid3
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
/*
 For GUIDE64
 * (C) 2020 Sadao Ikebe @bonyarou
 * modifyed by 2023 Kenji Yoshizawa
 */

#include QMK_KEYBOARD_H
#include "bootloader.h"

#include "nicola.h" // NICOLA親指シフト
#include "jtu.h"    // JIS keyboard on ANSI layout hardware

// Each layer gets a name for readability, which is then used in the keymap matrix below.
// The underscores don't mean anything - you can have a layer called STUFF or any other name.
// Layer names don't all need to be of the same length, obviously, and you can also skip them
// entirely and just use numbers.

enum keymap_layers {
  _QWERTY,
// NICOLA親指シフト
  _NICOLA, // NICOLA親指シフト入力レイヤー
// NICOLA親指シフト
  _FUNC
};

enum custom_keycodes {
  KC_EISU = NG_SAFE_RANGE,
  KC_KANA2
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /*
     * ┌───┬───┬───┬───┬───┬───┐    ┌───┬───┬───┬───┬───┬───┬───┬───┬───┐
     * │ Esc  │  1   │  2   │  3   │  4   │  5   │    │  6   │  7   │  8   │  9   │  0   │  -   │  =   │  ~   │  BS  │
     * ├───┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┐└─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴───┤
     * │ Tab      │  Q   │  W   │  E   │  R   │  T   │    │  Y   │  U   │  I   │  O   │  P   │  [   │  ]   │   \      │
     * ├─────┴┬──┴┬──┴┬──┴┬──┴┬──┴┐  └┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴─────┤
     * │ Ctrl       │  A   │  S   │  D   │  F   │  G   │    │  H   │  J   │  K   │  L   │  ;   │  '   │  Enter         │
     * ├──────┴┬──┴┬──┴┬──┴┬──┴┬──┴┐  └┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬───┬───┤
     * │ LShift       │  Z   │  X   │  C   │  V   │  B   │    │  N   │  M   │  ,   │  .   │  /   │RCtrl │  ↑  │RShift│
     * ├────┬──┴─┬─┴──┬┴───┴┬──┴──┬┘  ┌┴───┴┬──┴─┬─┴──┬┴───┼───┼───┼───┤
     * │ L1     │  LGUI  │  LAlt  │   Spc    │   Spc    │    │   Spc    │  Spc   │  RAlt  │   L1   │  ←  │  ↓  │  →  │
     * └────┴────┴────┴─────┴─────┘    └─────┴────┴────┴────┴───┴───┴───┘
     */
  [_QWERTY] = LAYOUT( \
      KC_ESC,  KC_1,    KC_2,    KC_3,  KC_4,    KC_5,      KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL, KC_GRV, KC_BSPC,\
      KC_TAB,   KC_Q,    KC_W,    KC_E,  KC_R,    KC_T,      KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,      \
      KC_LCTL,   KC_A,    KC_S,    KC_D,  KC_F,    KC_G,      KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,      \
      KC_LSFT,    KC_Z,    KC_X,    KC_C,  KC_V,    KC_B,      KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RCTL, KC_UP, KC_RSFT,      \
      MO(_FUNC),    KC_LGUI,   KC_LALT,        KC_EISU,  KC_SPC,  KC_SPC, KC_KANA2,         KC_RALT,MO(_FUNC), KC_LEFT,KC_DOWN,KC_RGHT),
    
// NICOLA親指シフト
  // デフォルトレイヤーに関係なくQWERTYで
  [_NICOLA] = LAYOUT( \
      KC_ESC,  NG_1,    NG_2,    NG_3,    NG_4,   NG_5,     NG_6,    NG_7,   NG_8,    NG_9,    NG_0,    NG_MINS,  NG_EQL, KC_GRV, KC_BSPC, \
      KC_TAB,  NG_Q,    NG_W,    NG_E,    NG_R,   NG_T,     NG_Y,    NG_U,   NG_I,    NG_O,    NG_P,    NG_LBRC,  NG_RBRC, NG_BSLS, \
      KC_LCTL, NG_A,    NG_S,    NG_D,    NG_F,   NG_G,     NG_H,    NG_J,   NG_K,    NG_L,    NG_SCLN, NG_QUOT,  KC_ENT,           \
      KC_LSFT, NG_Z,    NG_X,    NG_C,    NG_V,   NG_B,     NG_N,    NG_M,   NG_COMM, NG_DOT,  NG_SLSH, KC_LCTL,  KC_UP,   KC_RSFT, \
      MO(_FUNC), KC_LGUI, KC_LALT,               KC_TRNS, NG_SHFTL, NG_SHFTR, KC_TRNS,       KC_TRNS, MO(_FUNC), KC_TRNS,  KC_TRNS, KC_TRNS),


  // Function Layer
  [_FUNC] = LAYOUT( \
      KC_GRV,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,    KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,KC_TRNS,KC_DEL,\
      KC_TRNS, KC_TRNS,RGB_M_TW, KC_TRNS, RGB_M_R, RGB_TOG,   KC_HOME, KC_PGDN, KC_PGUP, KC_END,  KC_PSCR, KC_TRNS, KC_BRK, KC_TRNS,     \
      KC_TRNS, RGB_M_T, RGB_M_SW, KC_TRNS, RGB_M_P,  RGB_M_G,  KC_LEFT, KC_DOWN, KC_UP,   KC_RIGHT,KC_INS,  KC_DEL,  KC_ENT,             \
      KC_TRNS,LCTL(LALT(KC_DEL)),RGB_M_X,KC_TRNS,KC_TRNS,RGB_M_B,RGB_M_SN, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,KC_PGUP, KC_TRNS,   \
      MO(_FUNC),   KC_TRNS,  KC_TRNS,         KC_SPC,  KC_EISU,  KC_KANA2, KC_SPC,         KC_TRNS,MO(_FUNC), KC_HOME,KC_PGDN, KC_END)
};


void matrix_init_user(void) {
  // NICOLA親指シフト
  set_nicola(_NICOLA);
  // NICOLA親指シフト
}

// 開始LEDインデックス, 連続する個数, 光らせる色(ここではプリセットを使用)
#ifndef RGBLIGHT_LAYERS
  const rgblight_segment_t PROGMEM rgb_layer_1st[] = RGBLIGHT_LAYER_SEGMENTS(
    {0, 12, HSV_BLUE}, 
  );
  const rgblight_segment_t PROGMEM rgb_layer_2nd[] = RGBLIGHT_LAYER_SEGMENTS(
    {0, 12, HSV_GREEN}, 
  );
  const rgblight_segment_t PROGMEM rgb_layer_3rd[] = RGBLIGHT_LAYER_SEGMENTS(
    {0, 12, HSV_RED}, 
  );
  const rgblight_segment_t* const PROGMEM rgb_layers[] = RGBLIGHT_LAYERS_LIST(
    rgb_layer_1st,
    rgb_layer_2nd,
    rgb_layer_3rd,
  );
  

  void keyboard_post_init_user(void) {
    rgblight_layers = rgb_layers;
  }
  
  layer_state_t layer_state_set_user(layer_state_t state) {
    rgblight_set_layer_state(0, layer_state_cmp(state, _QWERTY));
    rgblight_set_layer_state(1, layer_state_cmp(state, _NICOLA));
    rgblight_set_layer_state(2, layer_state_cmp(state, _FUNC));
    return state;
  }
#endif // RGBLIGHT_LAYERS

// Initialize the 13th LED
void keyboard_post_init_user(void) {
    for(int i=0; i<RGBLED_NUM; ++i) {
	rgblight_setrgb_at(64, 64, 0, i);  // YELLOW
//        rgblight_setrgb_at(0, 0, 0, i);
    }
//    rgblight_sethsv_at(170,255,40, RGBLED_NUM-1); // the last LED = BLUE (NICOLA off)
    nicola_off();
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {

  switch (keycode) {
    case KC_EISU:
      if (record->event.pressed) {
        // NICOLA親指シフト
        send_string(SS_TAP(X_GRV)); // Win
//        send_string(SS_TAP(X_MHEN)); // Win
//        send_string(SS_TAP(X_LANG2)); // Mac
        nicola_off();
        // NICOLA親指シフト
      	for(int i=0; i<RGBLED_NUM; ++i) {
		rgblight_setrgb_at(0, 0, 64, i);  // BLUE
      	}
//        rgblight_sethsv_at(170,255,40, RGBLED_NUM-1); // the last LED = BLUE (NICOLA off)
      }
      return false;
      break;
    case KC_KANA2:
      if (record->event.pressed) {
        // NICOLA親指シフト
        send_string(SS_TAP(X_INT4)); // Win
//        send_string(SS_TAP(X_HENK)); // Win
//        send_string(SS_TAP(X_LANG1)); // Mac
        nicola_on();
        // NICOLA親指シフト
	for(int i=0; i<RGBLED_NUM; ++i) {
		rgblight_setrgb_at(0, 64, 0, i);  // GREEN
	}
//        rgblight_sethsv_at(85,255,40, RGBLED_NUM-1); // the last LED = GREEN (NICOLA on)

      }
      return false;
      break;
  }

  // NICOLA親指シフト
  bool a = true;
  if (nicola_state()) {
    nicola_mode(keycode, record);
    a = process_nicola(keycode, record);
  }
  if (a == false) return false;
  // NICOLA親指シフト

//   bool continue_process = process_jtu(keycode, record);
//   if (continue_process == false) {
//     return false;
//   }

  return true;
}
