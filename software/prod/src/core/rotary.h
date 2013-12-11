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
 * @date 30 nov 2013
 *
 * This file implements rotary encoder step decoding and debouncing.
 */

#include <stdint.h>
#include <hal/gpio.h>

typedef enum
{
  ROT_NO_STEP  = 0x00,
  ROT_STEP_CW  = 0x10,
  ROT_STEP_CCW = 0x20,
} rot_step_status;


/**
 * Declare a new rotary encoder
 *
 * This macro declares a rotary encoder attached to two specific pins. The
 * rotary encoder is named so that it can be refered to later on. The pin
 * symbols refer to symbols defined using the gpio abstraction layer (see
 * hal/gpio.h)
 *
 * @param name  The name of the rotary encoder
 * @param pina  The symbol for pin A
 * @param pinb  The symbol for pin B
 */
#define ROTARY(name,pina,pinb)                                         \
  static uint8_t rot_state_##name;                                     \
  static inline                                                        \
  rot_step_status rot_process_step_##name(void) {	      	       \
    uint8t_t pins_state = ((GET_PIN(pina) << 1) | GET_PIN(pinb))       \
    rot_state_##name = rot_ttable[rot_state_##name][pins_state];       \
    return (rot_state_##name & 0xF0);                                  \
  }


/**
 * Processes a single encoder step
 *
 * This macro resolves to a function that reads the specified rotary encoder's
 * input pins, debounces them and returns a value indicating whether a step was
 * taken and if so, in what direction.
 *
 * The macro can safely be used inside an interrupt routine. Typically, this
 * macro will either be called periodically from a timer or from a pin change
 * interrupt.
 *
 * @param  name The name of the rotary encoder for which to process a step
 * @return ROT_STEP_CW if a clockwise step was performed, ROT_STEP_CCW if a
 *         counter-clockwise step was performed and ROT_NO_STEP otherwise.
 */
#define rot_process_step(name) rot_process_step_##name()


/**
 * Transition table for the rotary encoder state machine. Not be used directly.
 */
extern const uint8_t rot_ttable[6][4];


#endif
