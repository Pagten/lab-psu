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

#include "hal/gpio.h" 
#include "core/scheduler.h"
#include "core/rotary.h"


#define ROT0A C,3
#define ROT0B C,2

static rotary rot0;

static inline
void init_pin_directions(void)
{
  SET_PIN_DIR_INPUT(ROT0A);
  SET_PIN_DIR_OUTPUT(ROT0B);
}


//ISR(PCINT_VECTOR(ROT0A))
PC_INTERRUPT_VECT(ROT0A)
{
  uint8_t input = (GET_PIN(ROT0A) << 1 | GET_PIN(ROT0B));
  switch (rot_process_step(&rot0, input)) {
  case ROT_STEP_CW:
    break;
  case ROT_STEP_CCW:
    break;
  default:
    break;
  }
}
//ISR(PCINT_VECTOR(ROT0B), ISR_ALIASOF(PCINT_VECTOR(ROT0A)));


int main(void)
{
  rot_init(&rot0);
  init_pin_directions();
  sched_init();

  PC_INTERRUPT_ENABLE(ROT0A);
  PC_INTERRUPT_ENABLE(ROT0B);
  sei(); // Enable interrupts

  while (1) {
    sched_exec();
  }
}
