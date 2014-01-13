/*
 * spi_master.h
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

#ifndef SPI_MASTER_H
#define SPI_MASTER_H

/**
 * @file spi_master.h
 * @author Pieter Agten <pieter.agten@gmail.com>
 * @date 29 oct 2013
 *
 * This file implements SPI master communication.
 */

#include <stdlib.h>
#include <stdint.h>

#include "core/timer.h"

typedef enum {
  SPIM_TRX_INIT_OK,
  SPIM_TRX_INIT_INVALID,
} spim_trx_init_status;

typedef enum {
  SPIM_TRX_QUEUE_OK,
  SPIM_TRX_QUEUE_INVALID_STATUS,
} spim_trx_queue_status;

typedef enum {
  SPIM_TRX_STATUS_INVALID,
  SPIM_TRX_STATUS_INITIAL,
  SPIM_TRX_STATUS_QUEUED,
  SPIM_TRX_STATUS_IN_TRANSMISSION,
  SPIM_TRX_STATUS_COMPLETED,
} spim_trx_status;

/**
 * The SPI transfer data structure.
 */
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


/**
 * Initializes the SPI master module.
 */
void spim_init(void);


/**
 * Initialize an SPI transfer data structure.
 * 
 * @param trx      The transfer data structure to initialize
 * @param ss_pin   The number of the pin connected to the SPI slave to address
 * @param ss_port  The port of the pin connected to the SPI slave to address
 * @param tx_buf   The data to be transmitted (can be NULL if tx_size is NULL)
 * @param tx_size  The number of bytes to be transmitted
 * @param rx_buf   The buffer into which to store the received data (can be 
 *                 NULL if rx_size is 0)
 * @param rx_size  The number of bytes to be received
 * @param delay    The minimum time to wait before transmitting each byte, to
 *                 give the SPI slave time to prepare its response
 * @return SPIM_TRX_INIT_OK if the transfer data structure was initialized
 *         succesfully or SPIM_TRX_INIT_EMPTY if both tx_size and rx_size are
 *         0.
 */
spim_trx_init_status
spim_trx_init(spi_trx *trx, uint8_t ss_pin, volatile uint8_t *ss_port,
	      uint8_t *tx_buf, size_t tx_size, uint8_t *rx_buf, size_t rx_size,
	      ticks_t delay);


/**
 * Get the status of an SPI transfer.
 * 
 * @param trx  The transfer data structure of which to get the status
 * @return The status of the given SPI transfer data structure.
 */
spim_trx_status spim_trx_get_status(spim_trx *trx);


/**
 * Queue an SPI transfer for execution.
 *
 * The transfer will be executed as soon as all previously queued transfers
 * have finished. The transfer must first be initialized with the 
 * spim_trx_init() function.
 *
 * @param trx  The SPI transfer to queue
 * @return SPIM_TRX_QUEUE_OK if the transfer was queued succesfully,
 *         SPIM_TRX_QUEUE_STATUS_INVALID if the status of the transfer is not
 *         SPIM_TRX_STATUS_INITIALIZED (hence the transfer is invalid or was
 *         already queued before).
 */
spim_trx_queue_status spim_trx_queue(spi_transfer *trx);


#endif
