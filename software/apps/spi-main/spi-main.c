/*
 * main-mcu.c
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
 * @file main-mcu.c
 * @author Pieter Agten (pieter.agten@gmail.com)
 * @date 23 Jul 2014
 *
 * This is the main file for the main-mcu part of a small LCD test program.
 */

#include "core/clock.h"
#include "core/process.h"
#include "core/spi_master.h"
#include "core/timer.h"
#include "hal/interrupt.h"
#include "hal/gpio.h"

#include <avr/io.h> // For fuses

// NOTE: the default fuse values defined in avr-libc are incorrect (see the 
// ATmega328p datasheet)
FUSES = 
{
  .extended = 0xFF,   // BOD disabled
  .high = FUSE_SPIEN, // SPIEN enabled
  .low = FUSE_CKSEL0, // Full swing crystal oscillator, slowly rising power
};

PROCESS(spi_transmitter);

#define DAC_CS B,1
#define LCD_CS B,2

#define DEBUG0 B,0

static inline
void init_pins(void)
{
  SET_PIN_DIR_OUTPUT(DAC_CS);
  SET_PIN(DAC_CS);

  SET_PIN_DIR_OUTPUT(LCD_CS);
  SET_PIN(LCD_CS);

  SET_PIN_DIR_OUTPUT(DEBUG0);
  CLR_PIN(DEBUG0);
}


PROCESS_THREAD(spi_transmitter)
{
  PROCESS_BEGIN();

  static timer tmr;
  static spim_trx_llp trx;
  static uint8_t tx_buf[1];

  timer_set(&tmr, CLOCK_SEC);
  spim_trx_init((spim_trx*)&trx);
  tx_buf[0] = 'a';

  while (true) {
    PROCESS_YIELD();
    PROCESS_WAIT_UNTIL(timer_expired(&tmr));

    if (! spim_trx_is_queued((spim_trx*)&trx)) {
      if (tx_buf[0] == 'z') {
	tx_buf[0] = 'a';
      } else {
	tx_buf[0] += 1;
      }

      spim_trx_llp_set(&trx, GET_BIT(LCD_CS), &GET_PORT(LCD_CS),
		       0x12, 1, tx_buf,
		       0, NULL, NULL);
      spim_trx_queue((spim_trx*)&trx);
    }
 

    timer_restart(&tmr);
  }

  PROCESS_END();
}


int main(void)
{
  ENABLE_INTERRUPTS();
  init_pins();
  clock_init();
  process_init();
  spim_init();

  process_start(&spi_transmitter);
  //  SET_PIN(DEBUG0);

  while (true) {
    process_execute();
  } 
}

