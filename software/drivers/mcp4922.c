/*
 * mcp4922.h
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
 * @file mcp4922.c
 * @author Pieter Agten (pieter.agten@gmail.com)
 * @date 27 dec 2013
 */

#include "core/spi_master.h"
#include "util/bit.h"
#include "hal/gpio.h"

#include "mcp4922.h"

#define SHDN 12
#define GA   13
#define BUF  14
#define CHB  15


void mcp4922_init()
{ }


inline
void mcp4922_pkt_init(mcp4922_pkt* pkt)
{
  spim_trx_init((spim_trx*)&(pkt->spim_trx));
}



void
mcp4922_pkt_set(mcp4922_pkt* pkt, uint8_t pin, port_ptr port,
		mcp4922_channel ch, uint16_t value)
{
  spim_trx_set_simple(&(pkt->spim_trx), pin, port,
		      2, pkt->data,      // tx_buf
		      0, NULL,           // rx_buf
		      NULL);             // process

  // This assumes MSB-first SPI data transfer
  pkt->data[0] = (value >> 8) & 0x0F;
  pkt->data[0] |= (_BV(GA) | _BV(SHDN)) >> 8;
  if (ch == MCP4922_CHANNEL_B) {
    pkt->data[0] |= _BV(CHB) >> 8;
  }
  pkt->data[1] = value & 0x00FF;
}


inline 
bool mcp4922_pkt_is_in_transmission(mcp4922_pkt* pkt)
{
  return spim_trx_is_in_transmission((spim_trx*)&(pkt->spim_trx));
}


mcp4922_pkt_queue_status
mcp4922_pkt_queue(mcp4922_pkt* pkt)
{
  spim_trx_queue_status stat;
  stat = spim_trx_queue((spim_trx*)&(pkt->spim_trx));
  if (stat != SPIM_TRX_QUEUE_OK) {
    return MCP4922_PKT_QUEUE_ERROR;
  }

  return MCP4922_PKT_QUEUE_OK;
}

