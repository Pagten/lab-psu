/*
 * psu-core.c
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
 * @file psu-core.c
 * @author Pieter Agten (pieter.agten@gmail.com)
 * @date 09 May 2014
 *
 * This is the firmware for the PSU's main MCU.
 */

#include <stdlib.h>
#include <util/delay.h>

#include "hal/gpio.h" 
#include "hal/fuses.h"
#include "hal/interrupt.h"
#include "core/io_monitor.h"
#include "core/spi_master.h"
#include "core/rotary.h"
#include "drivers/mcp4922.h"

#include "util/debug.h"

// NOTE: the default fuse values defined in avr-libc are incorrect (see the 
// ATmega328p datasheet)
FUSES = 
{
  .extended = 0xFF, // BOD disabled
  .high = FUSE_SPIEN, // SPIEN enabled
  .low = FUSE_CKSEL0, // Full swing crystal oscillator, slowly rising power
};


#define ROT0_A    D,0
#define ROT0_B    D,1
#define ROT0_PUSH D,2
#define DAC_CS    B,1


#define DAC_MIN 0x0000
#define DAC_MAX 0x0FFF
#define DAC_STEP 32

PROCESS(dacs_process);

static rotary rot0;
static iomon_event rot_tick;

#define EVENT_ROTARY_TICK        0x00
#define EVENT_MCP4922_WAS_BUSY   0x01

static inline
void init_pins(void)
{
  SET_PIN_DIR_INPUT(ROT0_A);
  SET_PIN_DIR_INPUT(ROT0_B);
  SET_PIN_DIR_INPUT(ROT0_PUSH);

  SET_PIN(DAC_CS);
  SET_PIN_DIR_OUTPUT(DAC_CS);
}


PROCESS_THREAD(dacs_process)
{
  PROCESS_BEGIN();
  static uint16_t dac_values[2] = { DAC_MIN, DAC_MIN };
  static uint8_t dac_idx = 0;
  static mcp4922_pkt mcp4922_pkt;

  mcp4922_pkt_init(&mcp4922_pkt);

  while (true) {
    PROCESS_WAIT_EVENT();

    if (ev == EVENT_ROTARY_TICK) {
      if (DEBOUNCED(data) & _BV(GET_BIT(ROT0_PUSH))) {
	// Rotary button pushed
	dac_idx = (dac_idx + 1) % 2;
      }

      uint8_t input = rot_input(DEBOUNCED(data), GET_BIT(ROT0_A),
				GET_BIT(ROT0_B));
      rot_step_status step = rot_process_step(&rot0, input);
      switch (step) {
      case ROT_STEP_CW:
	if (dac_values[dac_idx] <= DAC_MAX - DAC_STEP) {
	  dac_values[dac_idx] += DAC_STEP;
	} else {
	  dac_values[dac_idx] = DAC_MAX;
	}
	break;
      case ROT_STEP_CCW:
	if (dac_values[dac_idx] >= DAC_MIN + DAC_STEP) {
	  dac_values[dac_idx] -= DAC_STEP;
	} else {
	  dac_values[dac_idx] = DAC_MIN;
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
		      dac_idx, dac_values[dac_idx]);
      mcp4922_pkt_queue(&mcp4922_pkt);
    }
  }

  PROCESS_END();
}



int main(void)
{
  debug_init();

  init_pins();
  clock_init();
  process_init();
  spim_init();
  rot_init(&rot0);
  mcp4922_init();
  iomon_init();

  ENABLE_INTERRUPTS();

  process_start(&dacs_process);
  iomon_event_init(&rot_tick, PORT_PTR_TO_IOMON_PORT(&GET_PORT(ROT0_A)),
		   GET_PIN_MASK(GET_BIT(ROT0_A), GET_BIT(ROT0_B),
				GET_BIT(ROT0_PUSH)),
		   &dacs_process, EVENT_ROTARY_TICK);
  iomon_event_enable(&rot_tick);

  while (true) {
    process_execute();
  }
}
