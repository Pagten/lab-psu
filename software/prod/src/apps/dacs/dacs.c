/*
 * dacs.c
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


/**
 * @file dacs.c
 * @author Pieter Agten (pieter.agten@gmail.com)
 * @date 3 dec 2013
 *
 * This is the main file for a small DAC test program. It allows controlling
 * the DAC outputs using a rotary encoder.
 */

#include "config.h"
#include <hal/gpio.h> 

#include "scheduler.h"
#include "rotary.h"


#define ROT0A C,3
#define ROT0B C,2

ROTARY(ROT0,ROT0A,ROT0B)


static inline
void init_pin_directions(void)
{
  SET_PIN_DIRECTION_INPUT(ROT0A);
  SET_PIN_DIRECTION_OUTPUT(ROT0B);
}


ISR(PCINT_VECTOR(ROT0A))
{
  rot_process_step(rot0);
}
ISR(PCINT_VECTOR(ROT0B), ISR_ALIASOF(PCINT_VECTOR(ROT0A)));


void main(void) __attribute__((noreturn));
void main(void)
{
  init_pin_directions();
  sched_init();

  PCINT_ENABLE_GROUP(ROT0A);
  PCINT_ENABLE_GROUP(ROT0B);
  PCINT_ENABLE(ROT0A);
  PCINT_ENABLE(ROT0B);
  sei(); // Enable interrupts

  while (1) {
    sched_exec();
  }
}
