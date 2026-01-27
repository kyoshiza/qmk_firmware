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
#include "bootloader.h"
#include "nicola.h" // NICOLA親指シフト
#include "a2j/translate_ansi_to_jis.h"

enum keymap_layers {
  _QWERTY, // QWERTY LAYER
  _NICOLA, // NICOLA親指シフト入力レイヤー
  _FUNC1,  // FUNCTION LAYER 1
  _FUNC2,  // FUNCTION LAYER 2
  _LED     // LED SETTING LAYER
};

enum custom_keycodes {
  KC_EISU = NG_SAFE_RANGE,
  KC_KANA2
};

	    /*
     * ┌───┬───┬───┬───┬───┬───┐                ┌───┬───┬───┬───┬───┬───┐
     * │ Tab  │  Q   │  W   │  E   │  R   │  T   │                │  Y   │  U   │  I   │  O   │  P   │  BS  │
     * ├───┼───┼───┼───┼───┼───┤                ├───┼───┼───┼───┼───┼───┤
     * │ LCtl │  A   │  S   │  D   │  F   │  G   │                │  H   │  J   │  K   │  L   │  ;   │  '   │
     * ├───┼───┼───┼───┼───┼───┤                ├───┼───┼───┼───┼───┼───┤
     * │ LShf │  Z   │  X   │  C   │  V   │  B   │                │  N   │  M   │  ,   │  .   │  /   │ Esc  │
     * └───┴───┴─┬─┴──┬┴───┴─┬─┴──┐    ┌──┴─┬─┴───┴┬──┴─┬─┴───┴───┘
     *                     │  Fn1   │無変/親指左 │   Spc  │    │  Enter │ 変換/親指右│   Fn2  │
     *                     └────┴──────┴────┘    └────┴──────┴────┘
     */

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  // keymap for default
  [_QWERTY] = LAYOUT_universal(
     KC_TAB ,   KC_Q    ,  KC_W    ,  KC_E    ,  KC_R   ,  KC_T  ,        KC_Y    , KC_U    , KC_I    , KC_O     , KC_P     , KC_BSPC,
     KC_LCTL,   KC_A    ,  KC_S    ,  KC_D    ,  KC_F   ,  KC_G  ,        KC_H    , KC_J    , KC_K    , KC_L     , KC_SCLN  , KC_QUOT,
     KC_LSFT,   KC_Z    ,  KC_X    ,  KC_C    ,  KC_V   ,  KC_B  ,        KC_N    , KC_M    , KC_COMM , KC_DOT   , KC_SLSH  , KC_RSFT,
                KC_LGUI,   KC_LALT,   MO(_FUNC1),LT(_FUNC2,KC_EISU),LT(_LED,KC_SPC), LT(_LED,KC_ENT),KC_KANA2,XXXXXXX, XXXXXXX,MO(_FUNC2)
  ),
  // NICOLA親指シフト デフォルトレイヤーに関係なくQWERTYで
  [_NICOLA] = LAYOUT_universal(
     KC_TAB,    NG_Q,      NG_W,      NG_E,      NG_R,     NG_T,          NG_Y,     NG_U,     NG_I,     NG_O,      NG_P,      KC_BSPC,
     KC_LCTL,   NG_A,      NG_S,      NG_D,      NG_F,     NG_G,          NG_H,     NG_J,     NG_K,     NG_L,      NG_SCLN,   NG_QUOT,
     KC_LSFT,   NG_Z,      NG_X,      NG_C,      NG_V,     NG_B,          NG_N,     NG_M,     NG_COMM,  NG_DOT,    NG_SLSH,   KC_RSFT,
                _______  , _______ ,  MO(_FUNC1),NG_SHFTL, _______,       _______,  NG_SHFTR, XXXXXXX,  XXXXXXX,   MO(_FUNC2)
  ),

  [_FUNC1] = LAYOUT_universal(
     KC_ESC,    KC_1,      KC_2,      KC_3,      KC_4,      KC_5,         KC_6,     KC_7,     KC_8,     KC_9,      KC_0,      KC_DEL,
     KC_LCTL,   XXXXXXX,   KC_HOME,   KC_PGDN,   KC_PGUP,   KC_END,       KC_LEFT,  KC_DOWN,  KC_UP,    KC_RIGHT,  XXXXXXX,   KC_GRV,
     KC_LSFT,LCTL(LALT(KC_DEL)),_______,_______, _______,   _______,      KC_BTN1,  KC_BTN2,  KC_RALT,  KC_RGUI,   KBC_SAVE,  KC_TILD,
                KC_LGUI,   KC_LALT,   MO(_FUNC1),KC_EISU,LT(_LED,KC_SPC), LT(_LED,KC_ENT),KC_KANA2, XXXXXXX,XXXXXXX,MO(_FUNC2)
  ),

  [_FUNC2] = LAYOUT_universal(
     KC_ESC,    KC_EXLM,   KC_AT,     KC_HASH,   KC_DLR,    KC_PERC,      KC_CIRC,  KC_AMPR,  KC_ASTR,  KC_LPRN,   KC_RPRN,   KC_DEL,
     KC_LCTL,   XXXXXXX,   KC_LPRN,   KC_RPRN,   KC_LBRC,   KC_RBRC,      KC_MINS,  KC_EQL,   KC_LBRC,  KC_RBRC,   KC_BSLS,   KC_GRV,
     KC_LSFT,   KC_TRNS,   CPI_D1K,   CPI_D100,  CPI_I100,  CPI_I1K,      KC_UNDS,  KC_PLUS,  KC_LCBR,  KC_RCBR,   KC_PIPE,   QK_BOOT,
                KC_LGUI,   KC_LALT,   MO(_FUNC1),KC_EISU,LT(_LED,KC_SPC), LT(_LED,KC_ENT),KC_KANA2, XXXXXXX, XXXXXXX, MO(_FUNC2)
  ),

  [_LED] = LAYOUT_universal(
     RGB_TOG,   KC_F1,    KC_F2,      KC_F3,     KC_F4,     KC_F5,        KC_F6,    KC_F7,    KC_F8,    KC_F9,     KC_F10,    KC_F11,
     RGB_MOD,   RGB_HUI,  RGB_SAI,    RGB_VAI,   XXXXXXX,   SCRL_DVI,     RGB_M_T,  RGB_M_SW, RGB_M_P,  RGB_M_G,   KC_PSCR,   KC_F12,
     RGB_RMOD,  RGB_HUD,  RGB_SAD,    RGB_VAD,   XXXXXXX,   SCRL_DVD,     RGB_M_X,  RGB_M_B,  RGB_M_SN, RGB_M_TW,  KBC_SAVE,  KBC_RST,
                QK_BOOT,  KBC_RST,    MO(_FUNC1),KC_EISU,LT(_LED,KC_SPC), LT(_LED,KC_ENT),KC_KANA2, XXXXXXX, XXXXXXX, MO(_FUNC2)
  ),
};
// clang-format on

layer_state_t layer_state_set_user(layer_state_t state) {
    // Auto enable scroll mode when the highest layer is _LED
    keyball_set_scroll_mode(get_highest_layer(state) == _LED);
    
    // LOWER + RAISE = ADJUST のようなTri Layersを使う場合
    // これを先に書いておかないと3の色がおかしくなる
    // state = update_tri_layer_state(state, _QWERTY, _NICOLA, _FUNC1);

    uint8_t layer = biton32(state);
    switch (layer) {
        case _QWERTY:
//            rgblight_sethsv(HSV_RED);
            break;
        case _NICOLA:
            rgblight_sethsv(HSV_SPRINGGREEN);
            break;
        case _FUNC1:
            rgblight_sethsv(HSV_YELLOW);
            break;
        case _FUNC2:
            rgblight_sethsv(HSV_WHITE);
            break;
    }
    return state;
}

#ifdef OLED_ENABLE

#    include "lib/oledkit/oledkit.h"

void oledkit_render_info_user(void) {
    keyball_oled_render_keyinfo();
    keyball_oled_render_ballinfo();
    keyball_oled_render_layerinfo();
}
#endif


void matrix_init_user(void) {
  // NICOLA親指シフト
  set_nicola(_NICOLA);
  // NICOLA親指シフト
}


bool process_record_user(uint16_t keycode, keyrecord_t *record) {

  switch (keycode) {
    case KC_EISU:
      if (record->event.pressed) {
        // NICOLA親指シフト
        send_string(SS_TAP(X_INT5));  // Win (Muhenkan key)
//        send_string(SS_TAP(X_GRV));   // Win
//        send_string(SS_TAP(X_MHEN));  // Win
//        send_string(SS_TAP(X_LANG2)); // Mac
        nicola_off();
        // NICOLA親指シフト
      }
      return false;
      break;
    case KC_KANA2:
      if (record->event.pressed) {
        // NICOLA親指シフト
        send_string(SS_TAP(X_INT4)); // Win (Henkan key)
//        send_string(SS_TAP(X_HENK)); // Win
//        send_string(SS_TAP(X_LANG1)); // Mac
        nicola_on();
        // NICOLA親指シフト
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

// as kbd101 layer driver on windows recognizes the IME on/off key,
// we no longer need to disguise as 106 keyboard
//   bool continue_process = process_jtu(keycode, record);
//   if (continue_process == false) {
//     return false;
//   }
  return process_record_user_a2j(keycode, record);
  
//  return true;
}
