/*
 * knob.h
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

#ifndef KNOB_H
#define KNOB_H

/**
 * @file knob.h
 * @author Pieter Agten (pieter.agten@gmail.com)
 * @date 30 Jun 2014
 *
 * A knob is a 16-bit value that can be increased by turning a rotary 
 * encoder clockwise and decreased by turning it counterclockwise.
 */

#include <stdint.h>
#include "core/rotary.h"

typedef struct {
  rotary rot;
  uint16_t value;
  uint16_t small_step;
  uint16_t big_step;
} knob;

/**
 * Initialize a knob data structure.
 *
 * @param k          The knob data structure to initialize
 * @param small_step The amount the knob's value increases/decreases on a small
 *                   step increment/decrement
 * @param big_step   The amount the knob's value increases/decreases on a big
 *                   step increment/decrement
 */
void knob_init(knob* k, uint16_t small_step, uint16_t big_step);

/**
 * Signal that the state of a knob's rotary encoder has changed.
 *
 * This function should be called whenever a change is detected in the outputs
 * of the knob's rotary encoder. If the change in the rotary encoder's outputs
 * indicates the rotary encoder has turned, the knob's value will be updated
 * accordingly.
 *
 * @param k          The knob of which the rotary encoder's state has changed
 * @param rot_values The new state (output) of the rotary encoder
 */
void knob_update(knob* k, uint8_t rot_values);

/**
 * Return the current value of a knob.
 *
 * @param k The knob of which to return the current value
 * @return The current value of the given knob
 */
uint16_t knob_get_value(knob* k);

/**
 * Set (override) the current value of a knob.
 *
 * @param k     The knob of which to set the current value
 * @param value The value to set as the knob's current value
 */
void knob_set_value(knob* k, uint16_t value);

#endif
