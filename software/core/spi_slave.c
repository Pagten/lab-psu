/*
 * spi_slave.c
 *
 * Copyright 2014 Pieter Agten
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
 * @file spi_slave.c
 * @author Pieter Agten <pieter.agten@gmail.com>
 * @date 8 Jul 2014
 *
 * This file implements SPI slave communication using a simple link layer
 * protocol designed for request-response type messages.
 */

#include "spi_slave.h"
#include "spi_common.h"
#include "core/crc16.h"
#include "core/process.h"

// Note: SPIS_RX_BUF_SIZE must be between 0 and (255 - LLP_FOOTER_LENGTH)!

enum spis_trx_status {
  SPIS_TRX_IDLE,
  SPIS_TRX_RECEIVING_SIZE,
  SPIS_TRX_RECEIVING_PAYLOAD,
  SPIS_TRX_RECEIVING_FOOTER0,
  SPIS_TRX_RECEIVING_FOOTER1,
  SPIS_TRX_ERROR_WAIT,
  SPIS_TRX_SEND_ERROR_TYPE,
  SPIS_TRX_SEND_ERROR_SIZE,
  SPIS_TRX_WAITING_FOR_CALLBACK,
  SPIS_TRX_SEND_RESPONSE_SIZE,
  SPIS_TRX_SEND_RESPONSE_PAYLOAD,
  SPIS_TRX_SEND_FOOTER0,
  SPIS_TRX_SEND_FOOTER1,
  SPIS_TRX_COMPLETED
};

/**
 * The SPI transfer data structure.
 */
struct spis_trx {
  uint8_t rx_type;
  uint8_t rx_size;
  uint8_t rx_buf[SPIS_RX_BUF_SIZE];
  crc16_t crc;
  uint8_t rx_received;
  uint8_t* tx_buf;
  uint8_t tx_remaining;
  enum spis_trx_status status;
  uint8_t error_code;
};

static process* callback;
static struct spis_trx trx;

void spis_init()
{
  callback = NULL;
  rx_len = 0;
  trx.type = 0;
  trx.rx_size = 0;
  trx.crc = 0;
  trx.rx_received = 0;
  trx.tx_buf = NULL;
  trx.status = SPIS_TRX_IDLE;

  SPI_SET_PIN_DIRS_SLAVE();
  SPI_SET_ROLE_SLAVE();
  SPI_SET_DATA_ORDER_MSB();
  SPI_SET_MODE(0,0);
  SPI_SET_CLOCK_RATE_DIV_4();
  SPI_SET_DATA_REG(TYPE_RX_PROCESSING);
  PC_INTERRUPT_ENABLE(SPI_SS_PIN);
  SPI_ENABLE();
}

void spis_register_callback(process* p)
{
  ATOMIC_BLOCK {
    callback = p;
  }
}

spis_send_response_status
spis_send_response(uint8_t type, uint8_t* payload, uint8_t size)
{
  if (type > MAX_NORMAL_TYPE) {
    return SPIS_SEND_RESPONSE_INVALID_TYPE;
  }
  if (payload == NULL && size > 0) {
    return SPIS_SEND_RESPONSE_PAYLOAD_IS_NULL;
  }

  ATOMIC_BLOCK {
    if (trx.status != SPIS_TRX_WAITING_FOR_CALLBACK) {
      return SPIS_SEND_RESPONSE_NO_TRX_IN_PROGRESS;
    }

    do {
      SPI_SET_DATA_REG(type);
    } while (IS_SPI_WCOL_FLAG_SET());
    trx.tx_buf = payload;
    trx.tx_remaining = size;
    trx.status = SPIS_TRX_SEND_RESPONSE_SIZE;
  }
  return SPIS_SEND_RESPONSE_OK;
}


INTERRUPT(PC_INTERRUPT_VECT(SPI_SS_PIN))
{
  if (GET_PIN(SPI_SS_PIN)) {
    // The SS pin is high: the master is ending the transfer
    SPI_SET_DATA(TYPE_RX_PROCESSING);
    if (trx.status < SPIS_TRX_COMPLETED &&
	trx.status >= SPIS_TRX_WAITING_FOR_CALLBACK &&
	callback != NULL) {
      // Transfer was ended prematurely, after notifying the callback that a
      // message was received
      process_post_data(callback, SPIS_RESPONSE_ERROR, (process_data_t)trx);
    }
    trx.status = SPIS_TRX_IDLE;
  }
}


// TODO: should we check the write collision flag when we call
// SET_SPI_DATA_REG ?
INTERRUPT(SPI_TC_VECT)
{
  uint8_t data = SPI_GET_DATA_REG();
  switch (trx.status) {
  case SPIS_TRX_IDLE:
    // Master has started a new transfer, first byte is the message type
    trx.rx_type = data;
    trx.rx_received = 0;
    crc16_init(&(trx.crc));
    crc16_update(&(trx.crc), trx.rx_type);
    trx.status = SPIS_TRX_RECEIVING_SIZE;
    break;
  case SPIS_TRX_RECEIVING_SIZE:
    // Second byte is the message size
    trx.rx_size = data;
    if (trx.rx_size > SPIS_RX_BUF_SIZE) {
      // Message size too large for receive buffer
      trx.error_code = TYPE_ERR_MESSAGE_TOO_LARGE;
      trx.status = SPIS_TRX_ERROR_WAIT;
    } else {
      crc16_update(&(trx.crc), trx.rx_size);
      trx.status = SPIS_TRX_RECEIVING_PAYLOAD;
    }
    break;
  case SPIS_TRX_RECEIVING_PAYLOAD:
    if (trx.rx_received < trx.rx_size) {
      trx.rx_buf[trx.rx_received] = data;
      trx.rx_received += 1;
      crc16_update(&(trx.crc), data);
      break;
    }
    // Drop to SPIS_TRX_RECEIVING_FOOTER0
  case SPIS_TRX_RECEIVING_FOOTER0:
    trx.rx_received += 1;
    if ((trx.crc >> 8) == data) {
      trx.status = SPIS_TRX_RECEIVING_FOOTER1;
    } else {
      trx.error_code = TYPE_ERR_CRC_FAILURE;
      trx.status = SPIS_TRX_ERROR_WAIT;
    }
    break;
  case SPIS_TRX_RECEIVING_FOOTER1:
    trx.rx_received += 1;
    if ((trx.crc & 0x00FF) == data) {
      trx.status = SPIS_TRX_WAITING_FOR_CALLBACK;
      if (callback != NULL) {
	process_post_data(callback, SPIS_MESSAGE_RECEIVED, (process_data_t)trx);
	break;
      } else {
	trx.error_code = TYPE_ERR_NO_PROCESS_LISTENING;
      }
    } else {
      trx.error_code = TYPE_ERR_CRC_FAILURE;
    }
    // Drop to SPIS_TRX_SEND_ERROR_TYPE
  case SPIS_TRX_SEND_ERROR_TYPE:
    SPI_SET_DATA(trx.error_code);
    crc16_init(&(trx.crc)); // Re-use crc field for response crc calculation
    crc16_update(&(trx.crc), trx.error_code);
    trx.status = SPIS_TRX_SEND_ERROR_SIZE;
    break;
  case SPIS_TRX_ERROR_WAIT:
    trx.rx_received += 1;
    if (trx.rx_received == trx.rx_size + LLP_FOOTER_LENGTH - 1) {
      // If this is the second to last message byte (first footer byte), then
      // after receiving next byte we will start sending the error response.
      trx.status = SPIS_TRX_SEND_ERROR_TYPE;
    }
    break;
  case SPIS_TRX_SEND_ERROR_SIZE:
    SPI_SET_DATA(0); // No payload on error response
    crc16_update(&(trx.crc), 0);
    trx.status = SPIS_TRX_SEND_FOOTER0;
    break;
  case SPIS_TRX_WAITING_FOR_CALLBACK:
    // Do nothing
    break;
  case SPIS_TRX_SEND_RESPONSE_SIZE:
    SPI_SET_DATA(trx.tx_remaining);
    crc16_update(&(trx.crc), trx.tx_remaining);
    trx.status = SPIS_TRX_SEND_RESPONSE_PAYLOAD;
    break;
  case SPIS_TRX_SEND_RESPONSE_PAYLOAD:
    SPI_SET_DATA(*(trx.tx_buf));
    trx.tx_buf += 1;
    trx.tx_remaining -= 1;
    if (trx.tx_remaining == 0) {
      trx.status = SPIS_TRX_SEND_FOOTER0;
    }
    break;
  case SPIS_TRX_SEND_FOOTER0:
    SPI_SET_DATA((uint8_t)(trx.crc >> 8));
    trx.status = SPIS_TRX_SEND_FOOTER1;
    break;
  case SPIS_TRX_SEND_FOOTER1:
    SPI_SET_DATA((uint8_t)(trx.crc & 0x00FF));
    trx.status = SPIS_TRX_COMPLETED;
    break;
  case SPIS_TRX_COMPLETED:
    SPI_SET_DATA(TYPE_RX_PROCESSING);
    if (callback != NULL) {
      process_post_data(callback, SPIS_RESPONSE_TRANSMITTED,
			(process_data_t)trx);
    }
    break;
  }
}
