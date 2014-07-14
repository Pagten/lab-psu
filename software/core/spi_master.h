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
 * @date 29 Oct 2013
 *
 * This file provides SPI master communication. It provides two kinds of
 * transfers. The first is a simple data exchange in which a transmit buffer
 * is sent to a device and a receive buffer is simultaneously filled with the
 * the device's response (if any). The receive buffer is filled starting with
 * the first byte returned by the slave device, up to a pre-specified number
 * of bytes. The number of bytes to send can differ from the number of bytes
 * to receive, but both sizes have to be specified upfront.
 *
 * The other kind of transfer implements a simple link layer protocol designed
 * for request-response type messages. The protocol allows the slave device to
 * specify the length of its response (up to a pre-specified maximum). It also
 * allows the slave device to delay its response for up to 16 bytes after the
 * master has sent its message, which can be useful if the slave needs some
 * time to generate its response. The protocol consists of a master transmit
 * phase and master receive phase. The master transmit phase proceeds as
 * follows.
 *  1) The master initiates the transfer by pulling the slave's SS pin low.
 *  2) The master sends a two-byte header. The first byte of which is a user-
 *     specified identifier indicating the type of message being sent, and the
 *     second byte of which indicates the length of the payload that follows.
 *  3) The master sends the user-specified payload.
 *  4) The master sends a two-byte CRC checksum footer calculated over the
 *     two-byte header and the payload.
 *
 *  After these steps, the master receive phase begins, in which the slave
 *  should send its response. This phase proceeds as follows.
 *  1) The slave sends the value 0xFC as long it is not yet ready to send its
 *     actual response (the value indicates the slave is still calculating the
 *     response). The master will ignore these values (i.e., not copy them 
 *     into the receive buffer). If the slave has the response available
 *     immediately, it can skip this step.
 *  2) The slave sends a two-byte response header. The first byte of which is
 *     an identifier indicating the type of the response being sent. This byte
 *     MUST NOT be 0xFC. A response type of 0xFD indicates there is no process
 *     listening for incoming SPI messages on the slave. A response type of
 *     0xFE indicates a CRC checksum failure on the data sent by the master
 *     and a response type of 0xFF indicates the master payload is too large
 *     for the slave's receive buffer. The second header byte indicates the
 *     length of the response payload that follows.
 *  3) The slave sends the response payload.
 *  4) The slave sends a two-byte CRC checksum footer calculated over the
 *     two-byte response header and the response payload.
 *  The transfer ends successfully when (1) the master has sent its data
 *  according to the scheme above, (2) the master has received the slave's
 *  footer, (3) the slave's CRC checksum is correct and (4) the slave's
 *  response type is not 0xFD, 0xFE or 0xFF.
 *
 *  In order for the slave to have enough time to read its SPI receive
 *  register and set up its SPI transmit register, the master will wait at
 *  least 30us between each byte during the master transmit phase and at least
 *  40us between each byte during the master receive phase.
 *
 *  From the viewpoint of the master, the following exceptions can occur
 *  during this process:
 *   * The slave delays its response for more than 16 bytes after the master
 *     has sent its footer (by continually sending 0xFC)
 *   * The slave indicates a CRC checksum failure.
 *   * The slave indicates its receive buffer is too small for the payload
 *     sent by the master.
 *   * The response size indicated by the slave is larger than the maximum
 *     response size specified by the user.
 *   * The CRC checksum fails, indicating the response is corrupt.
 *  In each of these cases, the master will abort the transfer as soon as it
 *  detects the exception.
 *
 *
 * The follow figure depicts the packet format used by both the master and the
 * slave for the link layer protocol:
 *
 *          +--------------------------------------+
 *  byte 0  |   message/response type identifier   |
 *          +--------------------------------------+
 *  byte 1  |   message/response payload size (n)  |
 *          +--------------------------------------+
 *  byte 2  |                                      |
 *          /           payload (n bytes)          /
 *          |                                      |
 *          +--------------------------------------+
 * byte 2+n |           CRC16 (high byte)          |
 *          |           CRC16 (low byte)           |
 *          +--------------------------------------+
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>


#include "core/clock.h"
#include "core/process.h"

#define SPIM_NO_DELAY 0

typedef enum {
  SPIM_TRX_LLP_OK,
  SPIM_TRX_LLP_TX_BUF_IS_NULL,
  SPIM_TRX_LLP_RX_BUF_IS_NULL,
  SPIM_TRX_LLP_RX_BUF_TOO_SMALL,
} spim_trx_set_llp_status;

typedef enum {
  SPIM_TRX_SIMPLE_OK,
  SPIM_TRX_SIMPLE_TX_BUF_IS_NULL,
  SPIM_TRX_SIMPLE_RX_BUF_IS_NULL,
} spim_trx_set_simple_status;

typedef enum {
  SPIM_TRX_QUEUE_OK,
  SPIM_TRX_QUEUE_ALREADY_QUEUED,
} spim_trx_queue_status;


//struct spim_trx;
typedef struct spim_trx spim_trx;

/**
 * An SPI simple transfer data structure.
 */
typedef struct {
  uint8_t flags;
  uint8_t ss_mask;
  volatile uint8_t *ss_port;
  process* p;
  struct spim_trx* next;

  uint8_t tx_size;  
  uint8_t* tx_buf;
  uint8_t rx_size;
  uint8_t* rx_buf;
} spim_trx_simple;


/**
 * An SPI LLP transfer data structure.
 */
typedef struct {
  uint8_t flags_rx_delay_remaining;
  uint8_t ss_mask;
  volatile uint8_t *ss_port;  
  process* p;
  struct spim_trx* next;

  uint8_t tx_type;
  uint8_t tx_size;
  uint8_t* tx_buf;
  uint8_t rx_type;
  uint8_t rx_size;
  uint8_t* rx_buf;
} spim_trx_llp;


PROCESS_NAME(spim_trx_process);

/**
 * Initializes the SPI master module.
 *
 * Note: This module configure the SS pin as an output, which is required for
 * the SPI to operate in master mode (see the ATmega datasheet for details).
 * 
 * Dependencies that must be initialized first:
 *  * process
 *  * clock
 */
void spim_init(void);


/**
 * Initialize an SPI transfer data structure.
 * 
 * Every SPI transfer data structure must be initialized at least once before
 * passing it to one of the other functions of this module. This function
 * must not be called on transfers that are in the transfer queue.
 *
 * @param trx  The transfer data structure to initialize
 */
void spim_trx_init(spim_trx* trx);


/**
 * Configure an SPI transfer data structure for a simple data exchange.
 * 
 * This function should not be called on SPI transfers that are in
 * transmission.
 *
 * @param trx      The transfer data structure to configure
 * @param ss_pin   The number of the pin connected to the SPI slave to address
 * @param ss_port  The port of the pin connected to the SPI slave to address
 * @param tx_size  The number of bytes to be transmitted
 * @param tx_buf   The data to be transmitted (can be NULL if tx_size is 0)
 * @param rx_size  The number of bytes to be received  
 * @param rx_buf   The buffer into which to store the received data (must be
 *                 at least rx_size bytes, but can be NULL if rx_size is 0)
 * @param p        The process to notify when the transfer is complete
 * @return SPIM_TRX_SET_OK if the transfer structure was initialized succes-
 *         fully or SPIM_TRX_SET_INVALID if both tx_size and rx_size are 0. 
 */
spim_trx_set_simple_status
spim_trx_set_simple(spim_trx_simple* trx, uint8_t ss_pin,
		    volatile uint8_t* ss_port, uint8_t tx_size,
		    uint8_t* tx_buf, uint8_t rx_size, uint8_t* rx_buf,
		    process* p);

/**
 * Configure an SPI transfer data structure for a data exchange using the link
 * layer protocol describe at the top of this file.
 *
 * @param trx      The transfer data structure to configure
 * @param ss_pin   The number of the pin connected to the SPI slave to address
 * @param ss_port  The port of the pin connected to the SPI slave to address
 * @param tx_type  The message type identifier
 * @param tx_size  The size (in bytes) of the payload to transmit
 * @param tx_buf   The payload to be transmitted (can be NULL if tx_size is 0)
 * @param rx_max   The maximum size (in bytes) of the payload to receive
 * @param rx_buf   The buffer into which to store the received data (must be
 *                 at least rx_size bytes, but can be NULL if rx_size is 0)
 * @param p        The process to notify when the transfer is complete
 * @return SPIM_TRX_SET_OK if the transfer structure was initialized success-
 *         fully or SPIM_TRX_SET_INVALID if TODO
 */
spim_trx_set_llp_status
spim_trx_set_llp(spim_trx_llp* trx, uint8_t ss_pin, volatile uint8_t* ss_port,
		 uint8_t tx_type, uint8_t tx_size, uint8_t* tx_buf,
		 uint8_t rx_max, uint8_t* rx_buf, process* p);


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
 * @return SPIM_TRX_QUEUE_OK if the transfer was queued successfully, or
 *         SPIM_TRX_QUEUE_ALREADY_QUEUED if the packet is already in the
 *         transfer queue.
 */
spim_trx_queue_status spim_trx_queue(spim_trx* trx);


#endif
