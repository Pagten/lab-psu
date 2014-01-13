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
 * @date 6 jun 2013
 */

#include "lib/pt/pt.h"
#include "hal/spi.h"
#include "util/log.h"

#include "spi_master.h"

//TODO: declare timer

static uint8_t trx_queue_head;
static uint8_t trx_queue_tail;

void spim_init()
{
  trx_queue_head = NULL;
  trx_queue_tail = NULL;

  SPI_SET_PIN_DIRS_MASTER();
  SPI_SET_ROLE_MASTER();
  SPI_SET_DATA_ORDER_MSB();
  SPI_SET_MODE(0,0);
  SPI_SET_CLOCK_RATE_DIV_4();
  SPI_ENABLE();
}

typedef struct _spi_trx {
  spim_trx_status status;
  uint8_t ss_mask;
  volatile uint8_t *ss_port;
  uint8_t *tx_buf;
  size_t tx_remaining;
  uint8_t *rx_buf;
  size_t rx_remaining;
  ticks_t delay;
  struct _spi_trx *next;
} spi_trx;



spim_trx_init_status
spim_trx_init(spim_trx *trx, uint8_t ss_pin, volatile uint8_t *ss_port,
	      uint8_t *tx_buf, size_t tx_size, uint8_t *rx_buf, size_t rx_size,
	      ticks_t delay);
{
  if (tx_size == 0 && rx_size == 0) {
    trx->status = SPIM_TRX_STATUS_INVALID;
    return SPIM_TRX_INIT_EMPTY;
  }

  trx->status = SPIM_TRX_STATUS_INITIAL;
  trx->ss_mask = _bv8(ss_pin & 0x07);
  trx->ss_port = ss_port;
  trx->tx_buf = tx_buf;
  trx->tx_remaining = tx_size;
  trx->rx_buf = rx_buf;
  trx->rx_remaining = rx_size;
  trx->delay = delay;
  return SPIM_TRX_INIT_OK;
}

inline
spim_trx_status spim_trx_get_status(spim_trx *trx)
{
  return trx->status;
}

spim_trx_queue_status spim_trx_queue(spi_transfer *trx)
{
  if (trx->status != SPIM_TRX_STATUS_INITIAL) {
    return SPIM_TRX_QUEUE_INVALID_STATUS;
  }

  if (trx_queue_tail == NULL) {
    // Queue is empty
    trx_queue_head = trx_queue_tail = trx;
  } else {
    // Append to queue
    trx_queue_tail->next = trx;
  }
  trx->next = NULL;
  trx->status = SPIM_TRX_STATUS_QUEUED;
  return SPIM_TRX_QUEUE_OK;
}


PT_THREAD(spim_trx_thread(struct pt *pt))
{
  PT_BEGIN(pt);

  while (true) {
    // Wait until there's something in the queue
    PT_WAIT_WHILE(pt, trx_queue_head == NULL);

    // Update transfer status
    trx_queue_head->status = SPIM_TRX_STATUS_IN_TRANSMISSION;

    // Start transfer by pulling the slave select pin low
    *(trx_queue_head->ss_port) &= ~bv8(trx_queue_head->flags_ss_pin & 0x07);

    // Wait before sending first byte 
    timer_set(&trx_timer, trx_queue_head->delay);
    PT_WAIT_UNTIL(pt, timer_expired(&trx_timer));
    
    // Send first byte
    tx_byte(trx_queue_head);
    
    // Send/receive next bytes
    while (trx_queue_head->tx_remaining > 0 || 
	   trx_queue_head->rx_remaining > 0) {
      // Wait before sending/receiving each byte
      timer_set(&trx_timer, trx_queue_head->delay);
      PT_WAIT_UNTIL(pt, timer_expired(&trx_timer));
      
      // Make sure previous transfer is complete
      while (! SPI_INTERRUPT_FLAG_SET);

      rx_byte(trx_queue_head);
      tx_byte(trx_queue_head);
    }

    // Wait before ending transfer
    timer_set(&trx_timer, trx_queue_head->delay);
    PT_WAIT_UNTIL(pt, timer_expired(&trx_timer));

    // Make sure the transfer is complete
    while (! SPI_INTERRUPT_FLAG_SET);

    // End transfer by making the slave select pin high again
    *(trx_queue_head->ss_port) |= bv8(trx_queue_head->flags_ss_pin & 0x07);

    // Shift queue
    trx_queue_head = trx_queue_head->next;
    if (trx_queue_head == NULL) {
      trx_queue_tail = NULL;
    }

    // Update transfer status
    trx_queue_head->status = SPIM_TRX_STATUS_COMPLETED;

    // Yield CPU for other threads to ensure there is some time between making
    // the slave select pin high and pulling it low again for the next
    // transfer, which can be an issue if there are two subsequent transfers
    // to the same slave device.
    PT_YIELD(pt);
  }

  PT_END(pt);
}
