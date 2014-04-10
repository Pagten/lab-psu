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

#include "hal/gpio.h" 
#include "hal/fuses.h"
#include "hal/interrupt.h"
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


#define ROT0A C,3
#define ROT0B C,2
#define DAC_CS B,0

#define DAC_MIN 0x0000
#define DAC_MAX 0x0FFF
#define DAC_STEP 32

PROCESS(dacs_process);

static rotary rot0;

#define EVENT_ROTARY_STEP_TAKEN  0x00
#define EVENT_MCP4922_WAS_BUSY   0x01

static inline
void init_pins(void)
{
  SET_PIN_DIR_INPUT(ROT0A);
  SET_PIN_DIR_INPUT(ROT0B);

  SET_PIN_DIR_OUTPUT(DAC_CS);
  SET_PIN(DAC_CS);
}


INTERRUPT(PC_INTERRUPT_VECT(ROT0A))
{
  uint8_t input = ((GET_PIN(ROT0A) << 1) | GET_PIN(ROT0B));
  rot_step_status step = rot_process_step(&rot0, input);
  if (step == ROT_STEP_CW || step == ROT_STEP_CCW) {
    process_post_event(&dacs_process, EVENT_ROTARY_STEP_TAKEN, 
		       (process_data_t)step);
  }
}
#if PC_INTERRUPT_VECT(ROT0B) != PC_INTERRUPT_VECT(ROT0A)
INTERRUPT(PC_INTERRUPT_VECT(ROT0B), INTERRUPT_ALIAS(PC_INTERRUPT_VECT(ROT0A)));
#endif



PROCESS_THREAD(dacs_process)
{
  PROCESS_BEGIN();
  static uint16_t dac_value = DAC_MIN;
  static mcp4922_pkt mcp4922_pkt;

  mcp4922_pkt_init(&mcp4922_pkt);

  while (true) {
    PROCESS_WAIT_EVENT();

    if (ev == EVENT_ROTARY_STEP_TAKEN) {
      rot_step_status step = (rot_step_status)data;
  
      switch (step) {
      case ROT_STEP_CW:
	if (dac_value <= DAC_MAX - DAC_STEP) {
	  dac_value += DAC_STEP;
	} else {
	  dac_value = DAC_MAX;
	}
	break;
      case ROT_STEP_CCW:
	if (dac_value >= DAC_MIN + DAC_STEP) {
	  dac_value -= DAC_STEP;
	} else {
	  dac_value = DAC_MIN;
	}
	break;
      default:
	continue;
      }
    }

    if (mcp4922_pkt_is_in_transmission(&mcp4922_pkt)) {
      process_post_event(PROCESS_CURRENT(), EVENT_MCP4922_WAS_BUSY,
			 PROCESS_DATA_NULL);
    } else {
      mcp4922_pkt_set(&mcp4922_pkt, GET_BIT(DAC_CS), &GET_PORT(DAC_CS),
		      MCP4922_CHANNEL_A, dac_value);
      mcp4922_pkt_queue(&mcp4922_pkt);
    }
  }

  PROCESS_END();
}



int main(void)
{
  init_pins();
  clock_init();
  process_init();
  spim_init();
  rot_init(&rot0);
  mcp4922_init();

  PC_INTERRUPT_ENABLE(ROT0A);
  PC_INTERRUPT_ENABLE(ROT0B);
  ENABLE_INTERRUPTS();

  process_start(&dacs_process);

  while (true) {
    process_execute();
  }
}
