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
 * allows the slave device to delay its response for up to 31 bytes after the
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
 *  The slave should confirm reception of each byte by sending the value 0xF0
 *  in response. The slave can respond with a value different from 0xF0 to
 *  indicate various error conditions (see Table 1 below). The master must
 *  check each response and must abort the transfer if a value different from
 *  0xF0 is detected. Note that the first response from the slave is
 *  transmitted during the second SPI transfer, and the master should hence
 *  ignore the byte received from the slave during the first SPI transfer.
 *
 *  After these steps, the master receive phase begins, in which the slave
 *  sends its response. This phase proceeds as follows.
 *  5) The slave sends the value 0xF0 as long it is not yet ready to send its
 *     actual response (the value indicates the slave is preparing the
 *     response). The master must ignore these values (i.e., not copy them 
 *     into the receive buffer). The master must stay in this phase for 16 
 *     SPI byte transfers, or until the slave sends a value different from
 *     0xF0, whichever comes first. If the slave does not send a value
 *     different from 0xF0 within 31 SPI transfers, the master must abort the
 *     transfer. Otherwise, the first byte different from 0xF0 should be 
 *     considered the first response byte (see the next step).
 *  6) The slave sends its response, starting with a response header. The
 *     first byte of which indicates the response type. Values greater than
 *     0xF0 indicate various error conditions (see Table 1 below) while values
 *     lower than 0xF0 are valid user-defined response types. If the response
 *     type indicates an error, the master must end the transfer. Otherwise,
 *     the second response byte indicates the length of the payload that
 *     follows (see the next step).
 *  7) The slave sends the response payload.
 *  8) The slave sends a two-byte CRC checksum footer calculated over the
 *     two-byte response header and the response payload.
 *  The transfer ends successfully when (1) the master has sent its data
 *  according to the scheme above, (2) the master has received the slave's
 *  footer, (3) the slave's CRC checksum is correct and (4) no error condition
 *  was indicated by the slave.
 *
 *  In order for the slave to have enough time to read its SPI receive
 *  register and set up its SPI transmit register, the master must wait at
 *  least 40us between each byte during the master transmit phase and at least
 *  50us between each byte during the master receive phase.
 *
 *
 * Table 1: Error conditions that can be raised by the slave
 *  +-------------+----------------------------------------------------------+
 *  | Response    |                                                          |
 *  | type value  | Description                                              |
 *  +-------------+----------------------------------------------------------+
 *  |             | The slave is not ready to receive a message, because it  |
 *  | 0xF1        | is still processing a previously received message        |
 *  +-------------+----------------------------------------------------------|
 *  |             | The slave client process tried to send an invalid        |
 *  | 0xF2        | response                                                 |
 *  |-------------|----------------------------------------------------------|
 *  |             | The CRC checksum check of the message sent by the master |
 *  | 0xF3        | has failed                                               |
 *  |-------------|----------------------------------------------------------|
 *  |             | The message sent by the master is too large for the      |
 *  | 0xF4        | receive buffer of the slave                              |
 *  |-------------|----------------------------------------------------------|
 *  | 0xF5 - 0xFF | Reserved for future use                                  |
 *  +-------------+----------------------------------------------------------+
 *
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

#include "core/spi_common.h"
#include "core/clock.h"
#include "core/process.h"

typedef enum {
  SPIM_TRX_LLP_OK,
  SPIM_TRX_LLP_TX_BUF_IS_NULL,
  SPIM_TRX_LLP_RX_BUF_IS_NULL,
} spim_trx_llp_set_status;

typedef enum {
  SPIM_TRX_SIMPLE_OK,
  SPIM_TRX_SIMPLE_TX_BUF_IS_NULL,
  SPIM_TRX_SIMPLE_RX_BUF_IS_NULL,
} spim_trx_simple_set_status;

typedef enum {
  SPIM_TRX_QUEUE_OK,
  SPIM_TRX_QUEUE_ALREADY_QUEUED,
} spim_trx_queue_status;

typedef enum {
  SPIM_TRX_ERR_NONE = 0,

  // Errors detected master-side
  SPIM_TRX_ERR_RESPONSE_TOO_LARGE,
  SPIM_TRX_ERR_RESPONSE_CRC_ERROR,
  SPIM_TRX_ERR_RESPONSE_TIMEOUT,

  // Errors detected slave-side
  SPIM_TRX_ERR_SLAVE_UNKNOWN,
  SPIM_TRX_ERR_SLAVE_RESPONSE_INVALID = SPI_TYPE_ERR_SLAVE_RESPONSE_INVALID,
  SPIM_TRX_ERR_SLAVE_NOT_READY = SPI_TYPE_ERR_SLAVE_NOT_READY,
  SPIM_TRX_ERR_SLAVE_CRC_FAILURE = SPI_TYPE_ERR_CRC_FAILURE,
  SPIM_TRX_ERR_SLAVE_MSG_TOO_LARGE = SPI_TYPE_ERR_MESSAGE_TOO_LARGE,
} spim_trx_error_type;


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
  uint8_t rx_max;
  uint8_t rx_size;
  uint8_t* rx_buf;
  spim_trx_error_type error;
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
 * @return SPIM_TRX_SIMPLE_OK if the transfer structure was initialized
 *         successfully, SPIM_TRX_SIMPLE_TX_BUF_IS_NULL if tx_size is greater
 *         than 0 but tx_buf is NULL, SPIM_TRX_SIMPLE_RX_BUF_IS_NULL if
 *         rx_size is greater than 0 but rx_buf is NULL.
 */
spim_trx_simple_set_status
spim_trx_simple_set(spim_trx_simple* trx, uint8_t ss_pin,
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
 * @param tx_size  The size (in bytes) of the payload to transmit (must be <=
 *                 SPIM_MAX_TX_SIZE)
 * @param tx_buf   The payload to be transmitted (can be NULL if tx_size is 0)
 * @param rx_max   The maximum size (in bytes) of the payload to receive
 * @param rx_buf   The buffer into which to store the received data (must be
 *                 at least rx_size bytes, but can be NULL if rx_size is 0)
 * @param p        The process to notify when the transfer is complete
 * @return SPIM_TRX_LLP_OK if the transfer structure was initialized success-
 *         fully, SPIM_TRX_LLP_TX_BUF_IS_NULL if tx_size is greater than 0 but
 *         tx_buf is NULL, or SPIM_TRX_LLP_RX_BUF_IS_NULL if rx_size is
 *         greater than 0 but rx_buf is NULL.
 */
spim_trx_llp_set_status
spim_trx_llp_set(spim_trx_llp* trx, uint8_t ss_pin, volatile uint8_t* ss_port,
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

/**
 * Return the size of the transmit buffer of a given SPI transfer.
 *
 * @param trx The transfer of which to return the transmit buffer size
 * @return The transmit buffer size of the given transfer.
 */
uint8_t
spim_trx_llp_get_tx_size(spim_trx_llp* trx);

/**
 * Return the ransmit buffer of a given SPI transfer.
 *
 * @param trx The transfer of which to return the transmit buffer
 * @return The transmit buffer of the given transfer.
 */
uint8_t*
spim_trx_llp_get_tx_buf(spim_trx_llp* trx);

/**
 * Return the size of the receive buffer or the number of bytes received of a
 * given SPI transfer. If the given transfer has been completed successfully,
 * the return value indicates the number of bytes received, otherwise the
 * return value indicates the size of the receive buffer.
 *
 * @param trx The transfer of which to return the receive buffer size or the
 *            number of bytes received.
 * @return The size of the receive buffer or the number of byte received.
 */
uint8_t
spim_trx_llp_get_rx_size(spim_trx_llp* trx);

/**
 * Return the receive buffer of a given SPI transfer.
 *
 * @param trx The transfer of which to return the receive buffer
 * @return The receive buffer of the given transfer.
 */
uint8_t*
spim_trx_llp_get_rx_buf(spim_trx_llp* trx);

/**
 * Return the type of error encountered by a given SPI transfer.
 *
 * @param trx The transfer for which to get the error type
 * @return The type of error encountered by the given transfer
 */
spim_trx_error_type
spim_trx_llp_get_error_type(spim_trx_llp* trx);


#endif
