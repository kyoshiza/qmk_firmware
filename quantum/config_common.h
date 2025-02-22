/* Copyright 2015-2018 Jack Humbert
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

#pragma once

#ifndef __ASSEMBLER__
#    include "pin_defs.h"
#endif

/* diode directions */
#define COL2ROW 1
#define ROW2COL 2
#define EITHERWAY 3 /* Compensation for diode direction dissonance */
#define BOTHWAYS 4  /* Duplex-Matrix */

#include "song_list.h"
