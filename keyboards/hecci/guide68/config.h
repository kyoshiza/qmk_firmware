/*
Copyright 2012 Jun Wako <wakojun@gmail.com>
Copyright 2015 Jack Humbert

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

/* key matrix size */
#define MATRIX_ROWS 10
#define MATRIX_COLS 8



/* Set 0 if debouncing isn't needed */
#define DEBOUNCE 5

/* serial.c configuration for split keyboard */
// #define SPLIT_HAND_PIN B6

/* Mechanical locking support. Use KC_LCAP, KC_LNUM or KC_LSCR instead in keymap */
#define LOCKING_SUPPORT_ENABLE
/* Locking resynchronize hack */
#define LOCKING_RESYNC_ENABLE

#define RGBLIGHT_LAYERS /* enable RGBLIGHT function */
#define RGBLIGHT_MAX_LAYERS 3 /* maximum up to 32 */


/* 
#define LED_LAYOUT( \
    L00, L01, L02, L03, L04, L05,      R00, R01, R02, R03, R04, R05, R06, R07, R17, \
    L10, L11, L12, L13, L14, L15,      R10, R11, R12, R13, R14, R15, R16, R27, \
    L20, L21, L22, L23, L24, L25,      R20, R21, R22, R23, R24, R25, R26, \
    L30, L31, L32, L33, L34, L35,      R30, R31, R32, R33, R34, R35, R36, R37, \
    L40, L41, L42, L43, L44,           R40, R41, R43, R44, R45, R46, R47 \
  ) \
  { \
    { L00,   L01,   L02,   L03,   L04,   L05,   KC_NO, KC_NO }, \
    { L10,   L11,   L12,   L13,   L14,   L15,   KC_NO, KC_NO }, \
    { L20,   L21,   L22,   L23,   L24,   L25,   KC_NO, KC_NO }, \
    { L30,   L31,   L32,   L33,   L34,   L35,   KC_NO, KC_NO }, \
    { L40,   L41,   L42,   L43,   L44,   KC_NO, KC_NO, KC_NO }, \
    { R00,   R01,   R02,   R03,   R04,   R05,   R06,   R07   }, \
    { R10,   R11,   R12,   R13,   R14,   R15,   R16,   R17   }, \
    { R20,   R21,   R22,   R23,   R24,   R25,   R26,   R27   }, \
    { R30,   R31,   R32,   R33,   R34,   R35,   R36,   R37   }, \
    { R40,   R41,   KC_NO, R43,   R44,   R45,   R46,   R47   }  \
  }

#define RGBLIGHT_LED_MAP LED_LAYOUT( \
      5,  6, \
      4,  7, \
      3,  8, \
      2,  9, \
  0,  1,  10,  11)
#define RGBLIGHT_LED_MAP { 0, 1 , 2, 3, 4, 5, \
                          11, 10, 9, 8, 7, 6 }
*/

#ifndef IOS_DEVICE_ENABLE
  #define RGBLIGHT_LIMIT_VAL 180
  #define RGBLIGHT_VAL_STEP 17
#else
  #define RGBLIGHT_LIMIT_VAL 50
  #define RGBLIGHT_VAL_STEP 4
#endif
// #define RGBLIGHT_HUE_STEP 169
// #define RGBLIGHT_HUE_STEP 10
// #define RGBLIGHT_SAT_STEP 17

#if defined(RGBLIGHT_ENABLE) && !defined(IOS_DEVICE_ENABLE)
// USB_MAX_POWER_CONSUMPTION value for naked48 keyboard
//  120  RGBoff, OLEDoff
//  120  OLED
//  330  RGB 6
//  300  RGB 32
//  310  OLED & RGB 32
  #define USB_MAX_POWER_CONSUMPTION 400
#else
  // fix iPhone and iPad power adapter issue
  // iOS device need lessthan 100
  #define USB_MAX_POWER_CONSUMPTION 100
#endif
