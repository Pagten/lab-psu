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

spim_trx_queue_status spim_trx_queue(spim_trx* trx)
{
  if (spim_trx_is_queued(trx)) {
    return SPIM_TRX_QUEUE_ALREADY_QUEUED;
  }

  if (trx_queue_tail == NULL) {
    // Queue is empty
    trx_queue_head = trx_queue_tail = trx;
  } else {
    // Append to queue
    trx_queue_tail->next = trx;
  }
  trx->next = NULL;
  return SPIM_TRX_QUEUE_OK;
}

static inline 
void rx_byte(spim_trx* trx)
{
  if (trx->rx_remaining > 0) {
    *(trx->rx_buf) = SPI_GET_DATA_REG();
    trx->rx_buf += 1;
    trx->rx_remaining -= 1;
  }
}

static inline
void tx_byte(spim_trx* trx)
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


// As an alternative (more efficient) way of yielding, an event timer can be
// implemented (using a hardware timer) that posts an event when it expires.
// The PROCESS_WAIT_EVENT() function can be used to wait for such
// timers (this function should not post an event before yielding, because the
// timer will post an event when necessary).
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

    // Wait before sending first byte 
    timer_set(&trx_timer, trx_queue_head->delay);
    PROCESS_WAIT_UNTIL(timer_expired(&trx_timer));
    
    // Send first byte
    tx_byte(trx_queue_head);
    
    // Send/receive next bytes
    while (trx_queue_head->tx_remaining > 0 || 
	   trx_queue_head->rx_remaining > 0) {
      // Wait before sending/receiving each byte
      timer_restart(&trx_timer);
      PROCESS_WAIT_UNTIL(timer_expired(&trx_timer));
      
      // Make sure previous transfer is complete
      while (! IS_SPI_INTERRUPT_FLAG_SET());

      rx_byte(trx_queue_head);
      tx_byte(trx_queue_head);
    }

    // Wait before ending transfer
    timer_restart(&trx_timer);
    PROCESS_WAIT_UNTIL(timer_expired(&trx_timer));

    // Make sure the transfer is complete
    while (! IS_SPI_INTERRUPT_FLAG_SET());

    // End transfer by making the slave select pin high again
    *(trx_queue_head->ss_port) |= trx_queue_head->ss_mask;

    // Update transfer status
    trx_queue_head->in_transmission = false;

    // Shift queue
    trx_queue_head = trx_queue_head->next;
    if (trx_queue_head == NULL) {
      trx_queue_tail = NULL;
    }

    // Yield CPU for other threads to ensure there is some time between making
    // the slave select pin high and pulling it low again for the next
    // transfer, which can be an issue if there are two subsequent transfers
    // to the same slave device.
    PROCESS_YIELD();
  }

  PROCESS_END();
}
