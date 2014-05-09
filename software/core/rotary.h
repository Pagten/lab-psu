/*
 * rotary.h
 *
 * Copyright 2013 Pieter Agten
 *
 * This file is part of the lab-psu firmware.
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

#ifndef ROTARY_H
#define ROTARY_H

/**
 * @file rotary.h
 * @author Pieter Agten <pieter.agten@gmail.com>
 * @date 30 Nov 2013
 *
 * This file provides rotary encoder step decoding and debouncing. The
 * implementation is based on code provided by Ben Buxton at his blog
 * http://www.buxtronix.net/2011/10/rotary-encoders-done-properly.html .
 */

#include <stdint.h>

typedef struct
{
  uint8_t state;
} rotary;

typedef enum
{
  ROT_NO_STEP  = 0x00,
  ROT_STEP_CW  = 0x10,
  ROT_STEP_CCW = 0x20,
} rot_step_status;


/**
 * Initialize a rotary encoder
 *
 * This function initializes a rotary encoder structure. A rotary encoder must 
 * be initialized before it can be used with the other functions defined in
 * this file.
 */
void rot_init(rotary* rot);

/**
 * Select two bits from a byte and shift them towards the two least significant
 * positions of the output. This is a convenience function for passing an
 * arbitrary input to the rot_process_step() function.
 *
 * @param input  The raw input
 * @param bit_a  The bit to shift to the least significant position
 * @param bit_b  The bit to shift to the second least significant position
 * @param For an input of 0b'XXAXXBXX (the positions of A and B are arbitrary,
 *        but specified through the bit_a and bit_b parameters), the result
 *        will be 0b'000000BA.
 */
uint8_t rot_input(uint8_t input, uint8_t bit_a, uint8_t bit_b);

/**
 * Processes a single encoder step
 *
 * This function takes input from a rotary encoder, debounces it and returns a
 * value indicating whether a step was taken and if so, in what direction.
 *
 * The function can safely be used inside an interrupt routine. It will
 * typically either be called periodically from a timer or from a pin change
 * interrupt.
 *
 * @param rot   The rotary encoder for which to process a step
 * @param input The rotary encoder input, the LSB must have the value of pin A
 *              and LSB+1 must have the value of pin B (format 0b'000000BA).
 *              Inverting both bits has no influence on the outcome of the
 *              algorithm. 
 * @return ROT_STEP_CW if a clockwise step was performed, ROT_STEP_CCW if a
 *         counter-clockwise step was performed and ROT_NO_STEP otherwise.
 */
rot_step_status rot_process_step(rotary* rot, uint8_t input);


#endif
