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

#include <avr/sfr_defs.h>

#include "core/spi_master.h"
#include "util/bit.h"

#include "mcp4922.h"

#define SHDN 12
#define GA   13
#define BUF  14
#define CHB  15 


#ifndef MCP4922_PACKET_QUEUE_SIZE
#define MCP4922_PACKET_QUEUE_SIZE 4
#endif

struct packet {
  uint16_t data;
  mcp4922_set_callback cb;
  void* cb_data;
};

static struct packet packet_queue[MCP4922_PACKET_QUEUE_SIZE];
static uint8_t packet_queue_head;
static uint8_t packet_queue_tail;
static uint8_t packet_queue_count;


void mcp4922_init() {
  packet_queue_head = 0;
  packet_queue_tail = 0;
  packet_queue_count = 0;
}

static
size_t _callback(spim_cb_status status, void *data)
{
  struct packet *p = &packet_queue[packet_queue_head];
  mcp4922_status stat = (status == SPIM_TX_DONE) ? MCP4922_OK : MCP4922_ERROR;

  packet_queue_head = (packet_queue_head + 1) % MCP4922_PACKET_QUEUE_SIZE;
  packet_queue_count -= 1;
  if (p->cb != NULL) {
    p->cb(stat, p->cb_data);
  }

  return 0;
}


mcp4922_status mcp4922_set(volatile uint8_t *cs_port, uint8_t cs_pin,
			   uint16_t ch_value,mcp4922_set_callback cb, 
			   void* cb_data)
{
  struct packet *p = &packet_queue[packet_queue_tail];
  packet_queue_tail = (packet_queue_tail + 1) % MCP4922_PACKET_QUEUE_SIZE;
  packet_queue_count += 1;

  p->data = (ch_value & 0x8FFF) | _BV(GA) | _BV(SHDN);
  p->cb = cb;
  p->cb_data = cb_data;

  // Schedule new SPI transfer 
  spim_trx_status spim_status;
  spim_status = spim_trx((uint8_t*)&p->data, 2, 0, // TX_BUF, TX_SIZE, TX_DELAY
			 cs_port, bv8(cs_pin),     // SS_PORT, SS_MASK
			 NULL, 0,                  // RX_BUF, RX_SIZE
			 _callback, NULL);         // CB, CB_DATA
  if (spim_status != SPIM_OK) {
    return MCP4922_ERROR;
  }

  return MCP4922_OK;
}
