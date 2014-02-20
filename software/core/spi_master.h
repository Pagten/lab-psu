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

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>


#include "core/clock.h"
#include "core/process.h"

#define SPIM_NO_DELAY 0

typedef enum {
  SPIM_TRX_SET_OK,
  SPIM_TRX_SET_INVALID,
} spim_trx_set_status;

typedef enum {
  SPIM_TRX_QUEUE_OK,
  SPIM_TRX_QUEUE_ALREADY_QUEUED,
} spim_trx_queue_status;


/**
 * The SPI transfer data structure.
 */
typedef struct _spim_trx {
  bool in_transmission;
  uint8_t ss_mask;
  volatile uint8_t *ss_port;
  uint8_t* tx_buf;
  size_t tx_remaining;
  uint8_t* rx_buf;
  size_t rx_remaining;
  clock_time_t delay;
  struct _spim_trx* next;
} spim_trx;


PROCESS_NAME(spim_trx_process);

/**
 * Initializes the SPI master module.
 *
 * Dependencies that must be initialized first:
 *  * process
 *  * clock
 */
void spim_init(void);


/**
 * Set an SPI transfer data structure.
 * 
 * This function should not be called on SPI transfers that are in
 * transmission.
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
 * @return SPIM_TRX_SET_OK if the transfer data structure was initialized
 *         succesfully or SPIM_TRX_SET_INVALID if both tx_size and rx_size are
 *         0. 
 */
spim_trx_set_status
spim_trx_set(spim_trx* trx, uint8_t ss_pin, volatile uint8_t* ss_port,
	      uint8_t* tx_buf, size_t tx_size, uint8_t* rx_buf, size_t rx_size,
	      clock_time_t delay);


/**
 * Return whether an SPI transfer is in transmission.
 * 
 * The given SPI transfer must have been configured using the spim_trx_set()
 * function.
 *
 * @param trx  The SPI transfer data structure of which to get the status
 * @return true if the transfer is in transmission, false otherwise.
 */
bool spim_trx_is_in_transmission(spim_trx* trx);


/**
 * Return whether an SPI transfer is in the transfer queue.
 *
 * @param trx  The transfer for which to check if it is in the transfer queue
 * @return true if the transfer is in the queue, false otherwise.
 */
bool spim_trx_is_queued(spim_trx* trx);


/**
 * Queue an SPI transfer for execution.
 *
 * The transfer will be executed as soon as all previously queued transfers
 * have finished. The transfer must first be configured with the spim_trx_set()
 * function and should not already be in the transfer queue.
 *
 * @param trx  The SPI transfer to queue
 * @return SPIM_TRX_QUEUE_OK if the transfer was queued succesfully, or
 *         SPIM_TRX_QUEUE_ALREADY_QUEUED if the packet is already in the
 *         transfer queue.
 */
spim_trx_queue_status spim_trx_queue(spim_trx* trx);


#endif
