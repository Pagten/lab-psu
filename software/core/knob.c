/*
 * knob.c
 *
 * Copyright 2014 Pieter Agten
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


/**
 * @file knob.c
 * @author Pieter Agten (pieter.agten@gmail.com)
 * @date 30 Jun 2014
 *
 * A knob is a 16-bit value that can be increased by turning a rotary 
 * encoder clockwise and decreased by turning it counterclockwise.
 */

#include "knob.h"

#include <stdint.h>
#include "core/rotary.h"

#define KNOB_MAX UINT16_MAX
#define KNOB_MIN 0

void knob_init(knob* k, uint16_t small_step, uint16_t big_step)
{
  rot_init(&(k->rot));
  k->value = 0;
  k->small_step = small_step;
  k->big_step = big_step;
}

void knob_update(knob* k, uint8_t rot_input)
{
  rot_step_status step = rot_process_step(&(k->rot), rot_input);
  switch (step) {
  case ROT_STEP_CW:
    if (k->value <= KNOB_MAX - k->small_step) {
      k->value += k->small_step;
    } else {
      k->value = KNOB_MAX;
    }
    break;
  case ROT_STEP_CCW:
    if (k->value >= KNOB_MIN + k->small_step) {
      k->value -= k->small_step;
    } else {
      k->value = KNOB_MIN;
    }
    break;
  default:
    // Do nothing
    break;
  }
}

inline
uint16_t knob_get_value(knob* k)
{
  return k->value;
}

inline
void knob_set_value(knob* k, uint16_t value)
{
  k->value = value;
}
