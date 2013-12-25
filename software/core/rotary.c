/*
 * spi_master.c
 *
 * Copyright 2013 Ben Buxton, Pieter Agten
 *
 * This file is part of the lab-psu firmware and is based on the code at
 * http://www.buxtronix.net/2011/10/rotary-encoders-done-properly.html
 *
 * The firmware is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The firmware is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the firmware.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file rotary.c
 * @author Pieter Agten (pieter.agten@gmail.com)
 * @date 30 nov 2013
 *
 * This file implements rotary encoder step decoding and debouncing. It is
 * based on code provided by Ben Buxton at his blog
 * http://www.buxtronix.net/2011/10/rotary-encoders-done-properly.html .
 */

#include "rotary.h"

#define R_POS0 0x0
#define R_CCW0 0x1
#define R_CW0  0x2
#define R_POS1 0x3
#define R_CW1  0x4
#define R_CCW1 0x5

/* 
 * State diagram:
 * 
 * Position A B            
 *    0     0 0   |        ^
 *   1/2    1 0   |       /|\ ccw 
 *    1     1 1  \|/ cw    | 
 *   3/2    0 1   v        |
 *
 * Note that inverting the inputs A and B does not break the algorithm nor
 * does it change the direction of rotation.
 */
//             00                   01          10         11
static const uint8_t ttable[6][4] = {
/*R_POS0 */   {R_POS0,              R_CCW0,     R_CW0,     R_POS1},
/*R_CCW0 */   {R_POS0,              R_CCW0,     R_POS0,    R_POS1|ROT_STEP_CCW},
/*R_CW0  */   {R_POS0,              R_POS0,     R_CW0,     R_POS1|ROT_STEP_CW},
/*R_POS1 */   {R_POS0,              R_CW1,      R_CCW1,    R_POS1},
/*R_CW1  */   {R_POS0|ROT_STEP_CW,  R_CW1,      R_POS1,    R_POS1},
/*R_CCW1 */   {R_POS0|ROT_STEP_CCW, R_POS1,     R_CCW1,    R_POS1},
};


void rot_init(rotary* rot)
{
  rot->state = 0;
}


inline
rot_step_status rot_process_step(rotary* rot, uint8_t input)
{
  rot->state = ttable[rot->state][input];
  return rot->state & 0xF0; 
}
