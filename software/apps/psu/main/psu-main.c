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

#include "core/adc.h"
#include "core/process.h"
#include "core/timer.h"
#include "hal/gpio.h" 
#include "hal/fuses.h"
#include "hal/interrupt.h"
#include "core/spi_master.h"
#include "drivers/mcp4922.h"

#include "apps/psu/packets.h"

// NOTE: the default fuse values defined in avr-libc are incorrect (see the 
// ATmega328p datasheet)
FUSES = 
{
  .extended = 0xFF, // BOD disabled
  .high = FUSE_SPIEN, // SPIEN enabled
  .low = FUSE_CKSEL0, // Full swing crystal oscillator, slowly rising power
};

#define IOPANEL_UPDATE_RATE  (500 * CLOCK_MSEC)

#define DAC_CS      B,1
#define IOPANEL_CS  B,2

#define DAC_MIN 0x0000
#define DAC_MAX 0x0FFF

#define DAC_VOLTAGE_CHANNEL MCP4922_CHANNEL_A
#define DAC_CURRENT_CHANNEL MCP4922_CHANNEL_B

#define ADC_VOLTAGE_CHANNEL ADC_CHANNEL_0
#define ADC_CURRENT_CHANNEL ADC_CHANNEL_1

PROCESS(iopanel_update_process);

//#define EVENT_MCP4922_WAS_BUSY   0x01

#define PSU_FLAG_OUTPUT_ENABLED  0x01

static struct {
  uint8_t flags;
  uint16_t set_voltage;
  uint16_t set_current;
  adc voltage;
  adc current;
  adc line_voltage;
  adc temperature;
} psu_status;



PROCESS_THREAD(iopanel_update_process)
{
  PROCESS_BEGIN();

  static timer tmr;
  static spim_trx_llp trx;
  static struct iopanel_request request;
  static struct iopanel_response response;
  static mcp4922_pkt voltage_pkt;
  static mcp4922_pkt current_pkt;

  timer_set(&tmr, IOPANEL_UPDATE_RATE);
  spim_trx_init((spim_trx*)&trx);
  mcp4922_pkt_init(&voltage_pkt);
  mcp4922_pkt_init(&current_pkt);

  spim_trx_llp_set(&trx, GET_BIT(IOPANEL_CS), &GET_PORT(IOPANEL_CS),
		   IOPANEL_REQUEST_TYPE, sizeof(request), (uint8_t*)&request,
		   sizeof(response), (uint8_t*)&response, PROCESS_CURRENT());

  while (true) {
    timer_restart(&tmr);
    PROCESS_WAIT_UNTIL(timer_expired(&tmr));

    if (! spim_trx_is_queued((spim_trx*)&trx)) {
      request.flags = psu_status.flags;
      request.set_voltage = psu_status.set_voltage;
      request.set_current = psu_status.set_current;
      request.voltage = adc_get_measurement(&psu_status.voltage);
      request.current = adc_get_measurement(&psu_status.current);
      spim_trx_queue((spim_trx*)&trx);

      PROCESS_WAIT_EVENT_UNTIL(ev == SPIM_TRX_COMPLETED_SUCCESSFULLY ||
			       ev == SPIM_TRX_ERROR);
      if (ev == SPIM_TRX_ERROR || 
	  spim_trx_llp_get_rx_size(&trx) != sizeof(response)) {
	// Error occurred while communicating with IO panel. We will exit the
	// loop and try to communicate again.
	continue;
      }

      // Data exchanged successfully with IO panel. Now we will update the
      // psu state according to the values received from the IO panel.
      psu_status.set_voltage = response.set_voltage;
      psu_status.set_current = response.set_current;

      // Immediately update the DAC values according to the psu status
      // TODO: refactor this into a separate process?
      PROCESS_WAIT_UNTIL(! mcp4922_pkt_is_in_transmission(&voltage_pkt));
      mcp4922_pkt_set(&voltage_pkt, GET_BIT(DAC_CS), &GET_PORT(DAC_CS),
		      DAC_VOLTAGE_CHANNEL, psu_status.set_voltage >> 4);
      mcp4922_pkt_queue(&voltage_pkt);

      PROCESS_WAIT_UNTIL(! mcp4922_pkt_is_in_transmission(&current_pkt));
      // TODO: change mcp4922 interface so we don't have to repeat all
      // information whenever we want to transmit a packet
      mcp4922_pkt_set(&current_pkt, GET_BIT(DAC_CS), &GET_PORT(DAC_CS),
		      DAC_CURRENT_CHANNEL, psu_status.set_current >> 4);
      mcp4922_pkt_queue(&current_pkt);
    }
  }

  PROCESS_END();
}



int main(void)
{
  clock_init();
  process_init();
  spim_init();
  init_adc();
  mcp4922_init();

  // Enable ADC measurements
  adc_init(&psu_status.voltage, ADC_VOLTAGE_CHANNEL, ADC_256X_SAMPLING,
	   ADC_SKIP_0, NULL);
  adc_init(&psu_status.current, ADC_CURRENT_CHANNEL, ADC_256X_SAMPLING,
	   ADC_SKIP_0, NULL);
  adc_enable(&psu_status.voltage);
  adc_enable(&psu_status.current);


  ENABLE_INTERRUPTS();

  process_start(&iopanel_update_process);

  while (true) {
    process_execute();
  }
}
