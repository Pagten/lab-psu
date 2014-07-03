/*
 * spi_master.c
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
 * @file spi_master.c
 * @author Pieter Agten (pieter.agten@gmail.com)
 * @date 6 Jun 2013
 */

#include "spi_master.h"

#include <util/crc16.h>
#include "core/process.h"
#include "core/timer.h"
#include "hal/spi.h"
#include "util/bit.h"
#include "util/log.h"


PROCESS(spim_trx_process);

static timer trx_timer;
static spim_trx* trx_queue_head;
static spim_trx* trx_queue_tail;

#include "hal/gpio.h"


void spim_init(void)
{
  trx_queue_head = NULL;
  trx_queue_tail = NULL;

  SPI_SET_PIN_DIRS_MASTER();
  SPI_SET_ROLE_MASTER();
  SPI_SET_DATA_ORDER_MSB();
  SPI_SET_MODE(0,0);
  SPI_SET_CLOCK_RATE_DIV_32();
  SPI_ENABLE();

  process_start(&spim_trx_process);
}


void spim_trx_init(spim_trx* trx)
{
  trx->in_transmission = false;
  trx->ss_port = NULL;
}


spim_trx_set_status
spim_trx_set(spim_trx* trx, uint8_t ss_pin, volatile uint8_t* ss_port,
	     uint8_t* tx_buf, size_t tx_size, uint8_t* rx_buf, size_t rx_size,
	     clock_time_t delay)
{
  if (tx_size == 0 && rx_size == 0) {
    return SPIM_TRX_SET_INVALID;
  }

  trx->in_transmission = false;
  trx->ss_mask = bv8(ss_pin & 0x07);
  trx->ss_port = ss_port;
  trx->tx_buf = tx_buf;
  trx->tx_remaining = tx_size;
  trx->rx_buf = rx_buf;
  trx->rx_remaining = rx_size;
  trx->delay = delay;
  return SPIM_TRX_SET_OK;
}


/**
 * Configure an SPI transfer data structure for a data exchange using the link
 * layer protocol describe at the top of this file.
 *
 * @param trx      The transfer data structure to configure
 * @param ss_pin   The number of the pin connected to the SPI slave to address
 * @param ss_port  The port of the pin connected to the SPI slave to address
 * @param id       The message type identifier
 * @param tx_buf   The payload to be transmitted (can be NULL if tx_size is 0)
 * @param tx_size  The size (in bytes) of the payload to transmit
 * @param rx_buf   The buffer into which to store the received data (must be
 *                 at least rx_size bytes, but can be NULL if rx_size is 0)
 * @param rx_max   The maximum size (in bytes) of the payload to receive
 * @param p        The process to notify when the transfer is complete
 */
void
spim_trx_llp(spim_trx* trx, uint8_t ss_pin, volatile uint8_t* ss_port,
	     uint8_t id, uint8_t* tx_buf, size_t tx_size, uint8_t* rx_buf,
	     size_t rx_max, process* p)
{
  if (tx_buf == NULL) {
    tx_size = 0;
  }
  if (rx_buf == NULL) {
    rx_max = 0;
  }
  trx->flags_rx_delay_remaining = MAX_RX_DELAY;
  trx->ss_mask = bv8(ss_pin & 0x07);
  trx->ss_port = ss_port;
  trx->id = id;
  trx->tx_buf = tx_buf;
  trx->tx_remaining = tx_size;
  trx->rx_buf = rx_buf;
  trx->rx_max = rx_max;
}



inline
bool spim_trx_is_in_transmission(spim_trx* trx)
{
  return trx->in_transmission;
}

bool spim_trx_is_queued(spim_trx* trx)
{
  spim_trx* t = trx_queue_head;
  while (t != NULL) {
    if (t == trx) return true;
    t = t->next;
  }
  return false;
}

spim_trx_queue_status
spim_trx_queue(spim_trx* trx)
{
  if (spim_trx_is_queued(trx)) {
    return SPIM_TRX_QUEUE_ALREADY_QUEUED;
  }

  if (trx_queue_tail == NULL) {
    // Queue is empty
    trx_queue_head = trx;
  } else {
    // Append to queue
    trx_queue_tail->next = trx;
  }
  trx_queue_tail = trx;
  trx->next = NULL;
  return SPIM_TRX_QUEUE_OK;
}

static inline 
void rx_data_byte(spim_trx* trx)
{
  if (trx->rx_remaining > 0) {
    *(trx->rx_buf) = SPI_GET_DATA_REG();
    trx->rx_buf += 1;
    trx->rx_remaining -= 1;
  }
}

static inline
void tx_data_byte(spim_trx* trx)
{
  if (trx->tx_remaining > 0) {
    SPI_SET_DATA_REG(*(trx->tx_buf));
    trx->tx_buf += 1;
    trx->tx_remaining -= 1;
  } else {
    // Transmit null byte
    SPI_SET_DATA_REG(0);
  }
}

static inline
void tx_byte(uint8_t byte)
{
  do {
    SPI_SET_DATA_REG(byte);
  } while (IS_SPI_WRITE_COLLISION_FLAG_SET()); 
}

static inline
void rx_byte(spim_trx* trx)
{
  switch (trx->state) {

  }
  if (trx->state == WAITING_FOR_RX_HEADER) {
    uint8_t byte = SPI_GET_DATA_REG();
    //   if (byte != 
  }
}

static inline
void shift_trx_queue(void)
{
  trx_queue_head = trx_queue_head->next;
  if (trx_queue_head == NULL) {
    trx_queue_tail = NULL;
  }
}

PROCESS_THREAD(spim_trx_process)
{
  PROCESS_BEGIN();

  while (true) {
    // Wait until there's something in the queue
    PROCESS_WAIT_WHILE(trx_queue_head == NULL);

    // Update transfer status
    trx_queue_head->in_transmission = true;

    // Start transfer by pulling the slave select pin low
    *(trx_queue_head->ss_port) &= ~(trx_queue_head->ss_mask);

    // Send first header byte (message type id)
    tx_byte(trx_queue_head->id);
    trx_queue_head->crc = crc16_update(crc16_update(CRC16_INIT_VAL,
						    trx_queue_head->id),
				       trx_queue_head->tx_remaining);
    // Send second header byte (message size)
    tx_byte(trx_queue_head->tx_remaining);
    PROCESS_YIELD();
  
    // Send message bytes
    while (trx_queue_head->tx_remaining > 0) {
      tx_byte(*(trx_queue_head->tx_buf));
      trx_queue_head->crc = crc16_update(trx_queue_head->crc,
					 *(trx_queue_head->tx_buf));
      trx_queue_head->tx_buf += 1;
      trx_queue_head->tx_remaining -= 1;
      PROCESS_YIELD();
    }
    
    // Send CRC footer bytes
    tx_byte(trx_queue_head->crc >> 8);
    tx_byte(trx_queue_head->crc & 0x00FF);


    // TODO: start receiving data

    // Make sure the last byte has been transmitted
    while (! IS_SPI_INTERRUPT_FLAG_SET());
    // End transfer by making the slave select pin high again
    *(trx_queue_head->ss_port) |= trx_queue_head->ss_mask;

    // Update transfer status
    trx_queue_head->in_transmission = false;

    // Shift queue
    shift_trx_queue();

    // Yield CPU for other threads to ensure there is some time between making
    // the slave select pin high and pulling it low again for the next
    // transfer, which can be an issue if there are two subsequent transfers
    // to the same slave device.
    PROCESS_YIELD();
  }

  PROCESS_END();
}
