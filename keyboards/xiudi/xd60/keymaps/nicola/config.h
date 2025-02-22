/* Copyright 2018-2019 eswai <@eswai>
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
 For XD64 VER3.0 split-spacebar
 modification by Sadao Ikebe
 */

#pragma once
#include "config_common.h"

#undef PRODUCT
#define PRODUCT         XD60 3.0 NICOLA Oyayubi Shift

#define LAYOUT_62_ansi_2space( \
      K00, K01, K02,  K03,  K04,   K05, K06,  K07,   K08,   K09,  K0A, K0B, K0C,  K0D,   \
      K10, K11, K12,  K13,  K14,   K15, K16,  K17,   K18,   K19,  K1A, K1B, K1C,  K1D,   \
      K20, K21, K22,  K23,  K24,   K25, K26,  K27,   K28,   K29,  K2A, K2B,       K2D,   \
      K30, K32, K33,  K34,  K35,   K36, K37,  K38,   K39,   K3A,  K3B, K47, K3D,  K3C,   \
      K40, K41, K42,               K46, K45,                K4A,  K4B, K48, K4C,  K4D    \
) { \
    { K00, K01, K02, K03,   K04,   K05, K06,  K07,   K08,   K09,  K0A, K0B, K0C,  K0D }, \
    { K10, K11, K12, K13,   K14,   K15, K16,  K17,   K18,   K19,  K1A, K1B, K1C,  K1D }, \
    { K20, K21, K22, K23,   K24,   K25, K26,  K27,   K28,   K29,  K2A, K2B, KC_NO,K2D }, \
    { K30,KC_NO,K32, K33,   K34,   K35, K36,  K37,   K38,   K39,  K3A, K3B, K3C,  K3D }, \
    { K40, K41, K42, KC_NO, KC_NO, K45, K46,  K47,   K48,   KC_NO,K4A, K4B, K4C,  K4D }  \
}

// Change LED settings in order to make 13th LED NICOLA-mode indicator
// #define RGBLED_NUM
#define RGBLIGHT_ANIMATIONS
// #define RGBLED_NUM 13
