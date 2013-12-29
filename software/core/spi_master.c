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

#include "config.h"
#include "core/scheduler.h"
#include "hal/spi.h"
#include "util/log.h"

#include "spi_master.h"

#ifndef SPIM_TRX_QUEUE_SIZE
#warning "SPIM_TRX_QUEUE_SIZE undefined in config file, using default value"
#define SCHED_TRX_QUEUE_SIZE  4
#endif


struct transfer {
  volatile uint8_t *ss_port;
  uint8_t ss_mask;
  uint8_t *tx_pos;
  size_t tx_remaining;
  uint8_t *rx_pos;
  size_t rx_remaining;
  ticks_t delay;
  spim_trx_callback cb;
  void *cb_data;
};

static struct transfer trx_queue[SPIM_TRX_QUEUE_SIZE];
static uint8_t trx_queue_head;
static uint8_t trx_queue_tail;
static uint8_t trx_queue_count;

void spim_init()
{
  trx_queue_head = 0;
  trx_queue_tail = 0;
  trx_queue_count = 0;

  SPI_SET_ROLE_MASTER;
  SPI_SET_DATA_ORDER_MSB;
  SPI_SET_MODE(0,0);
  SPI_SET_CLOCK_RATE_div_4;
  SPI_ENABLE;
}

static inline void tx_byte(struct transfer *trx)
{
  if (trx->tx_remaining > 0) {
    SET_SPI_DATA_REG(*(trx->tx_pos));
    trx->tx_pos += 1;
    trx->tx_remaining -= 1;
    if (trx->tx_remaining == 0 && trx->cb != 0) {
      trx->cb(SPIM_TX_DONE, trx->cb_data);
    }
  } else if (trx->rx_remaining > 0) {
    // Transmit null byte
    SET_SPI_DATA_REG(0);
  }
}

static inline void rx_byte(struct transfer *trx)
{
  if (trx->rx_remaining > 0) {
    *(trx->rx_pos) = GET_SPI_DATA_REG;
    trx->rx_pos += 1;
    trx->rx_remaining -= 1;
    if (trx->rx_remaining == 0 && trx->cb != 0) {
      trx->rx_remaining = trx->cb(SPIM_RX_DONE, trx->cb_data);
    }
  }
}

static void task_trx_byte(void *data);
static void task_trx_start(void *data)
{
  struct transfer *trx = &trx_queue[trx_queue_head];

  // Pull slave select pin(s) low
  *(trx->ss_port) &= (uint8_t)(~trx->ss_mask);

  // Transmit first byte
  tx_byte(trx);

  sched_schedule_status task_scheduled;
  task_scheduled = sched_schedule(trx->delay, task_trx_byte, NULL);
  if (task_scheduled != SCHED_OK) {
    LOG_ERROR("SPIM: couldn't shedule first task_trx_byte";)
    trx->cb(SPIM_ERR_SCHED_FAILED, trx->cb_data);
  }
}

static void task_trx_byte(void *data)
{
  struct transfer *trx = &trx_queue[trx_queue_head];

  // Wait until previous byte complete (should be immediately)
  while (! SPI_INTERRUPT_FLAG_SET);

  if (trx->rx_remaining == 0 && trx->tx_remaining == 0) {
    // Last byte has been processed last time, let's end the transfer

    // Set slave select output high
    *(trx->ss_port) |= trx->ss_mask;

    // Shift ring buffer
    trx_queue_head = (trx_queue_head + 1) % SPIM_TRX_QUEUE_SIZE;
    trx_queue_count -= 1;

    if (trx_queue_count > 0) {
      // Schedule next transfer (if any)
      sched_schedule_status task_scheduled;
      task_scheduled = sched_schedule(0, task_trx_start, NULL);
      if (task_scheduled != SCHED_OK) {
        LOG_ERROR("SPIM: couldn't schedule next transfer");
	trx_queue[trx_queue_head].cb(SPIM_ERR_SCHED_FAILED, trx->cb_data);
      }
    }   
  } else {
    // Transmit/receive next byte
    rx_byte(trx);
    tx_byte(trx);

    // Schedule next byte
    sched_schedule_status task_scheduled;
    task_scheduled = sched_schedule(trx->delay, task_trx_byte, data);
    if (task_scheduled != SCHED_OK && trx->cb != 0) {
      LOG_ERROR("SPIM: couldn't schedule next byte");
      trx->cb(SPIM_ERR_SCHED_FAILED, trx->cb_data);
    }
  }
}

spim_trx_status spim_trx(uint8_t *tx_buf, size_t tx_size, ticks_t trx_delay,
                         volatile uint8_t *ss_port, uint8_t ss_mask,
                         uint8_t *rx_buf, size_t rx_size,
                         spim_trx_callback trx_cb, void *trx_cb_data)
{
  if (trx_queue_count == SPIM_TRX_QUEUE_SIZE) {
    return SPIM_QUEUE_FULL;
  }

  // Reserve slot in transfer queue
  struct transfer *trx = &trx_queue[trx_queue_tail];
  trx_queue_tail = (trx_queue_tail + 1) % SPIM_TRX_QUEUE_SIZE;
  trx_queue_count += 1;

  // Fill in slot in transfer queue
  trx->ss_port = ss_port;
  trx->ss_mask = ss_mask;
  trx->tx_pos = tx_buf;
  trx->tx_remaining = tx_size;
  trx->rx_pos = rx_buf;
  trx->rx_remaining = rx_size;
  trx->delay = trx_delay;
  trx->cb = trx_cb;
  trx->cb_data = trx_cb_data;

  // Schedule new transfer if no transfer in progress
  if (trx_queue_count == 1) {
    sched_schedule_status task_scheduled;
    task_scheduled = sched_schedule(0, task_trx_start, NULL);
    if (task_scheduled != SCHED_OK) {
      LOG_ERROR("SPIM: couldn't schedule new transfer");
      trx->cb(SPIM_ERR_SCHED_FAILED, trx->cb_data);
    }
  }
  return SPIM_OK;
}
