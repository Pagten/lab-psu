/*
 * psu-iopanel.c
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
 * @file psu-iopanel.c
 * @author Pieter Agten (pieter.agten@gmail.com)
 * @date 30 Jun 2014
 *
 * This is the firmware for the PSU's IO-panel.
 */

#include <stdlib.h>

#include "hal/gpio.h" 
#include "hal/fuses.h"
#include "hal/interrupt.h"
#include "core/io_monitor.h"
#include "core/spi_master.h"
#include "core/rotary.h"

#include "util/debug.h"

// NOTE: the default fuse values defined in avr-libc are incorrect (see the 
// ATmega328p datasheet)
FUSES = 
{
  .extended = 0xFF, // BOD disabled
  .high = FUSE_SPIEN, // SPIEN enabled
  .low = FUSE_CKSEL0, // Full swing crystal oscillator, slowly rising power
};


#define VOLTAGE_SMALL_STEP  0x0080
#define VOLTAGE_BIG_STEP    0X0800
#define CURRENT_SMALL_STEP  0x0080
#define CURRENT_BIG_STEP    0X0800


#define ROTV_A    D,0
#define ROTV_B    D,1
#define ROTV_PUSH D,2

#define ROTC_A    D,0
#define ROTC_B    D,1
#define ROTC_PUSH D,2


PROCESS(inputs_process);

static knob knob_v;
static knob knob_c;

static iomon_event rot_tick;

#define INPUT_EVENT        0x00

static inline
void init_pins(void)
{
  SET_PIN_DIR_INPUT(ROTV_A);
  SET_PIN_DIR_INPUT(ROTV_B);
  SET_PIN_DIR_INPUT(ROTV_PUSH);

  SET_PIN_DIR_INPUT(ROTC_A);
  SET_PIN_DIR_INPUT(ROTC_B);
  SET_PIN_DIR_INPUT(ROTC_PUSH);
}


PROCESS_THREAD(inputs_process)
{
  PROCESS_BEGIN();
  
  while (true) {
    PROCESS_WAIT_EVENT();
    if (ev != INPUT_EVENT)
      continue;

    uint8_t input_v = rot_input(DEBOUNCED(data), GET_BIT(ROTV_A), GET_BIT(ROTV_B));
    knob_input(&knob_v, input_v);

    uint8_t input_c = rot_input(DEBOUNCED(data), GET_BIT(ROTC_A), GET_BIT(ROTC_B));
    knob_input(&knob_c, input_c);
  }

  PROCESS_END();
}



int main(void)
{
  init_pins();
  clock_init();
  process_init();
  spim_init();
  knob_init(&knob_v, VOLTAGE_SMALL_STEP, VOLTAGE_BIG_STEP);
  knob_init(&knob_c, CURRENT_SMALL_STEP, CURRENT_BIG_STEP);
  iomon_init();

  ENABLE_INTERRUPTS();

  process_start(&inputs_process);
  iomon_event_init(&rot_tick, PORT_PTR_TO_IOMON_PORT(&GET_PORT(ROT0_A)),
		   GET_PIN_MASK(GET_BIT(ROT0_A), GET_BIT(ROT0_B),
				GET_BIT(ROT0_PUSH)),
		   &dacs_process, EVENT_ROTARY_TICK);
  iomon_event_enable(&rot_tick);

  while (true) {
    process_execute();
  }
}
