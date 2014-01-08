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

#include <stdlib.h>
#include <util/delay.h>

#include "config.h"

#include "hal/gpio.h" 
#include "hal/fuses.h"
#include "hal/interrupt.h"
#include "core/scheduler.h"
#include "core/spi_master.h"
#include "core/rotary.h"
#include "drivers/mcp4922.h"

// NOTE: the default fuse values defined in avr-libc are incorrect (see the 
// ATmega328p datasheet)
FUSES = 
{
  .extended = 0xFF, // BOD disabled
  .high = FUSE_SPIEN, // SPIEN enabled
  .low = FUSE_CKSEL0, // Full swing crystal oscillator, slowly rising power
};

#define DEBUG0 B,1
#define DEBUG1 B,2
#define DEBUG2 D,0


#define ROT0A C,3
#define ROT0B C,2
#define DAC_CS B,0
#define DAC_CS_PORT &PORTB
#define DAC_CS_PIN  0 

#define DAC_MAX 0x0FFF
#define DAC_STEP 32

static rotary rot0;

static inline
void init_pins(void)
{
  SET_PIN_DIR_OUTPUT(DEBUG0);
  SET_PIN_DIR_OUTPUT(DEBUG1);
  SET_PIN_DIR_OUTPUT(DEBUG2);
  CLR_PIN(DEBUG0);
  CLR_PIN(DEBUG1);
  CLR_PIN(DEBUG1);

  SET_PIN_DIR_INPUT(ROT0A);
  SET_PIN_DIR_INPUT(ROT0B);

  SET_PIN_DIR_OUTPUT(DAC_CS);
  SET_PIN(DAC_CS);
}


INTERRUPT(PC_INTERRUPT_VECT(ROT0A))
{
  static uint16_t dac_value = 0;

  uint8_t input = ((GET_PIN(ROT0A) << 1) | GET_PIN(ROT0B));
  switch (rot_process_step(&rot0, input)) {
  case ROT_STEP_CW:
    TGL_PIN(DEBUG0);
    if (dac_value <= DAC_MAX - DAC_STEP) {
      dac_value += DAC_STEP;
    } else {
      dac_value = DAC_MAX;
    }
    break;
  case ROT_STEP_CCW:
    TGL_PIN(DEBUG1);
    if (dac_value >= DAC_STEP) {
      dac_value -= DAC_STEP;
    } else {
      dac_value = 0;
    }
    break;
  default:
    return;
    }
  mcp4922_set(DAC_CS_PORT, DAC_CS_PIN, false, dac_value, NULL, NULL);
}
#if PC_INTERRUPT_VECT(ROT0B) != PC_INTERRUPT_VECT(ROT0A)
INTERRUPT(PC_INTERRUPT_VECT(ROT0B), INTERRUPT_ALIAS(PC_INTERRUPT_VECT(ROT0A)));
#endif

int main(void)
{
  init_pins();
  sched_init();
  rot_init(&rot0);
  spim_init();
  mcp4922_init();

  PC_INTERRUPT_ENABLE(ROT0A);
  PC_INTERRUPT_ENABLE(ROT0B);
  ENABLE_INTERRUPTS();

  //  SET_PIN(DEBUG0);
  // SET_PIN(DEBUG1);
  while (1) {
    sched_exec();
  }
}
