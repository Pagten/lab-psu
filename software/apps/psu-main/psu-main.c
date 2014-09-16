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

#include "hal/gpio.h" 
#include "hal/fuses.h"
#include "hal/interrupt.h"
#include "core/spi_master.h"
#include "drivers/mcp4922.h"

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

PROCESS(iopanel_process);

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

#define IOPANEL_REQUEST_TYPE 0x01
struct iopanel_request {
  uint8_t flags;
  uint16_t voltage;
  uint16_t current;
};

struct iopanel_response {
  uint8_t flags;
  uint16_t voltage;
  uint16_t current;
};

#define DAC_VOLTAGE_CHANNEL MCP4922_CHANNEL_A
#define DAC_CURRENT_CHANNEL MCP4922_CHANNEL_B

PROCESS_THREAD(iopanel_process)
{
  PROCESS_BEGIN();

  static timer tmr;
  static spim_trx_llp trx;
  static iopanel_request tx_buf;
  static iopanel_response rx_buf;
  static mcp4922_pkt voltage_pkt;
  static mcp4922_pkt current_pkt;

  tx_buf.flags = 0;
  tx_buf.voltage = DAC_MIN;
  tx_buf.current = DAC_MIN;
  timer_set(&tmr, CLOCK_SEC);
  spim_trx_init((spim_trx*)&trx);
  mcp4922_pkt_init(&mcp4922_pkt);

  while (true) {
    timer_restart(&tmr);
    PROCESS_WAIT_UNTIL(timer_expired(&tmr));

    if (! spim_trx_is_queued((spim_trx*)&trx)) {
      spim_trx_llp_set(&trx, GET_BIT(LCD_CS), &GET_PORT(LCD_CS),
		       IOPANEL_REQUEST_TYPE, sizeof(tx_buf), &tx_buf,
		       sizeof(rx_buf), &rx_buf, PROCESS_CURRENT());
      spim_trx_queue((spim_trx*)&trx);

      PROCESS_WAIT_EVENT(ev == SPIM_TRX_COMPLETED_SUCCESSFULLY ||
			 ev == SPIM_TRX_ERROR);
      if (ev == SPIM_TRX_ERROR) {
	continue;
      }
      if (spim_trx_llp_get_rx_size(&trx) != sizeof(rx_buf)) {
	continue;
      }

      PROCESS_WAIT_UNTIL(! mcp4922_pkt_is_in_transmission(&voltage_pkt));
      mcp4922_pkt_set(&voltage_pkt, GET_BIT(DAC_CS), &GET_PORT(DAC_CS),
		      DAC_VOLTAGE_CHANNEL, rx_buf.voltage >> 4);
      mcp4922_pkt_queue(&voltage_pkt);

      PROCESS_WAIT_UNTIL(! mcp4922_pkt_is_in_transmission(&current_pkt));
      mcp4922_pkt_set(&current_pkt, GET_BIT(DAC_CS), &GET_PORT(DAC_CS),
		      DAC_CURRENT_CHANNEL, rx_buf.current >> 4);
      mcp4922_pkt_queue(&current_pkt);

      tx_buf.voltage = rx_buf.voltage;
      tx_buf.current = rx_buf.current;
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
  mcp4922_init();

  ENABLE_INTERRUPTS();

  process_start(&iopanel_process);

  while (true) {
    process_execute();
  }
}
