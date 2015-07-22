/*
 * control.c
 *
 * Copyright 2015 Pieter Agten
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
 * @file control.c
 * @author Pieter Agten (pieter.agten@gmail.com)
 * @date 22 Jul 2015
 */

#include "control.h"

#define DAC_CS      B,1
#define ADC_VOLTAGE_CHANNEL ADC_CHANNEL_0
#define ADC_CURRENT_CHANNEL ADC_CHANNEL_1

#define CTRL_EVENT_SET_OUTPUT_CHANGED 0

PROCESS(ctrl_process);

static uint16_t channel_output[CTRL_NB_CHANNELS];

static const adc adcs[CTRL_NB_CHANNELS];
static const mcp4922_channel ch_to_dac[] =
{
  MCP4922_CHANNEL_A, // VOLTAGE CHANNEL
  MCP4922_CHANNEL_B, // CURRENT CHANNEL
};


void ctrl_init(void)
{
  adc_init(&adcs[CTRL_CH_VOLTAGE0], ADC_VOLTAGE_CHANNEL, ADC_RESOLUTION_15BIT,
	   ADC_SKIP_0, NULL);
  adc_enable(&adcs[CTRL_CH_VOLTAGE0]);

  adc_init(&adcs[CTRL_CH_CURRENT0], ADC_CURRENT_CHANNEL, ADC_RESOLUTION_15BIT,
	   ADC_SKIP_0, NULL);
  adc_enable(&adcs[CTRL_CH_CURRENT0]);

  process_start(&ctrl_process);
}


void ctrl_set_output(ctrl_channel ch, uint16_t val)
{
  if (ch < CTRL_NB_CHANNELS && val != channel_output[ch]) {
    channel_output[ch] = val;
    process_send_event(&ctrl_process, CTRL_EVENT_SET_OUTPUT_CHANGED,
		       (process_data_t)ch);
  }
}


inline uint16_t
ctrl_get_input(ctrl_channel ch)
{
  if (ch >= CTRL_NB_CHANNELS) {
    return 0;
  }

  return adc_get_value(&adcs[ch]);
}


PROCESS_THREAD(ctrl_process)
{
  PROCESS_BEGIN();

  static mcp4922_pkt packet;
  static ctrl_channel ch;

  mcp4922_pkt_init(&packet);

  while (true) {
    PROCESS_WAIT_EVENT_UNTIL(ev == CTRL_EVENT_SET_OUTPUT_CHANGED && 
			     !mcp4922_pkt_is_in_transmission(&packet));
    ch = (ctrl_channel)data;
    mcp4922_pkt_set(&packet, GET_BIT(DAC_CS), &GET_PORT(DAC_CS),
		    ch_to_dac[ch], channel_output[ch]);
    mcp4922_pkt_queue(&packet);
  }
}
