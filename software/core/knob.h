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


void knob_init(knob* k, uint16_t small_step, uint16_t big_step);

void knob_input(knob* k, uint8_t rot_input);

uint16_t knob_get_value(knob* k);

void knob_set_value(knob* k, uint16_t value);


#end if
