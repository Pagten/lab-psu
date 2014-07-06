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
#include "core/crc16.h"
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

#define RX_DELAY_REMAINING_MASK   ((1 << 4) - 1)
#define TRX_QUEUED_BIT           7
#define TRX_IN_TRANSMISSION_BIT  6
#define TRX_USE_LLP_BIT          5

void spim_init(void)
{
  trx_queue_head = NULL;
  trx_queue_tail = NULL;

  SPI_SET_PIN_DIRS_MASTER();
  SPI_SET_ROLE_MASTER();
  SPI_SET_DATA_ORDER_MSB();
  SPI_SET_MODE(0,0);
  SPI_SET_CLOCK_RATE_DIV_4();
  SPI_ENABLE();

  process_start(&spim_trx_process);
}


void spim_trx_init(spim_trx* trx)
{
  trx->flags_rx_delay_remaining = 0;
}


spim_trx_simple_status
spim_trx_simple(spim_trx* trx, uint8_t ss_pin, volatile uint8_t* ss_port,
		uint8_t* tx_buf, size_t tx_size, uint8_t* rx_buf,
		size_t rx_size, process* p)
{
  if (tx_buf == NULL && tx_size > 0) {
    return SPIM_TRX_SIMPLE_TX_BUF_IS_NULL;
  }
  if (rx_buf == NULL && rx_size > 0) {
    return SPIM_TRX_SIMPLE_RX_BUF_IS_NULL;
  }
  trx->flags_rx_delay_remaining = 0;
  trx->ss_mask = bv8(ss_pin & 0x07);
  trx->ss_port = ss_port;
  trx->tx_buf = tx_buf;
  trx->tx_remaining = tx_size;
  trx->rx_buf = rx_buf;
  trx->rx_remaining = rx_size;
  trx->p = p;
  return SPIM_TRX_SIMPLE_OK;
}


spim_trx_llp_status
spim_trx_llp(spim_trx* trx, uint8_t ss_pin, volatile uint8_t* ss_port,
	     uint8_t id, uint8_t* tx_buf, size_t tx_size, uint8_t* rx_buf,
	     size_t rx_buf_size, process* p)
{
  if (tx_buf == NULL && tx_size > 0) {
    return SPIM_TRX_LLP_TX_BUF_IS_NULL;
  }
  if (rx_buf == NULL) {
    return SPIM_TRX_LLP_RX_BUF_IS_NULL;
  }
  if (rx_buf_size <= LLP_HEADER_LENGTH) {
    return SPIM_TRX_LLP_RX_BUF_TOO_SMALL;
  }
  trx->flags_rx_delay_remaining = _BV(TRX_USE_LLP_BIT) | MAX_RX_DELAY;
  trx->ss_mask = bv8(ss_pin & 0x07);
  trx->ss_port = ss_port;
  trx->id = id;
  trx->tx_buf = tx_buf;
  trx->tx_remaining = tx_size;
  trx->rx_buf = rx_buf;
  trx->rx_remaining = rx_buf_size - LLP_HEADER_LENGTH;
  trx->p = p;
  return SPIM_TRX_LLP_OK;
}



inline
bool spim_trx_is_in_transmission(spim_trx* trx)
{
  return trx->flags_rx_delay_remaining & _BV(TRX_IN_TRANSMISSION_BIT);
}

static inline
void trx_set_in_transmission(spim_trx* trx, bool v)
{
  if (v) {
    trx->flags_rx_delay_remaining |= _BV(TRX_IN_TRANSMISSION_BIT);
  } else {
    trx->flags_rx_delay_remaining &= ~_BV(TRX_IN_TRANSMISSION_BIT);    
  }
}

inline
bool spim_trx_is_queued(spim_trx* trx)
{
  return trx->flags_rx_delay_remaining & _BV(TRX_QUEUED_BIT);
}

static inline
void trx_set_queued(spim_trx* trx)
{
  if (v) {
    trx->flags_rx_delay_remaining |= _BV(TRX_QUEUED_BIT);
  } else {
    trx->flags_rx_delay_remaining &= ~_BV(TRX_QUEUED_BIT);    
  }
}

static inline
uint8_t get_rx_delay_remaining(spim_trx* trx)
{
  return trx->flags_rx_delay_remaining & RX_DELAY_REMAINING_MASK;
}

/**
 * It is only allowed to call this function if get_rx_delay_remaining(trx)
 * is greater than 0!
 */
static inline
void decrement_rx_delay_remaining(spim_trx* trx)
{
  trx->flags_rx_delay_remaining -= 1;
}

spim_trx_queue_status
spim_trx_queue(spim_trx* trx)
{
  if (spim_trx_is_queued(trx)) {
    return SPIM_TRX_QUEUE_ALREADY_QUEUED;
  }

  trx_set_queued(trx_queue_head, true);
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
void tx_byte(uint8_t byte)
{
  do {
    SPI_SET_DATA_REG(byte);
  } while (IS_SPI_WRITE_COLLISION_FLAG_SET()); 
}

static inline
void tx_dummy_byte()
{
  tx_byte(0);
}

static inline
uint8_t read_response_byte()
{
  // Wait for transfer to complete
  while (! IS_SPI_INTERRUPT_FLAG_SET());
  return SPI_GET_DATA_REG();
}

static inline
void shift_trx_queue(void)
{
  trx_queue_head = trx_queue_head->next;
  if (trx_queue_head == NULL) {
    trx_queue_tail = NULL;
  }
}

static
void end_transfer(process_event_t ev)
{
  if (trx_queue_head->p != NULL) {
    process_post_event(trx_queue_head->p, ev, (process_data_t)trx_queue_head);
  }

  // Make the slave select pin high
  *(trx_queue_head->ss_port) |= trx_queue_head->ss_mask;

  // Update transfer status
  trx_set_in_transmission(trx_queue_head, false);
  trx_set_queued(trx_queue_head, false);
  
  // Shift transfer queue for next transfer
  shift_trx_queue();
}

PROCESS_THREAD(spim_trx_process)
{
  PROCESS_BEGIN();

  while (true) {
    PROCESS_YIELD();
    // Wait until there's something in the queue
    PROCESS_WAIT_WHILE(trx_queue_head == NULL);

    // Update transfer status
    trx_set_in_transmisstion(trx_queue_head, true);

    // Start transfer by pulling the slave select pin low
    *(trx_queue_head->ss_port) &= ~(trx_queue_head->ss_mask);

    if (trx->flags_rx_delay_remaining & _BV(TRX_USE_LLP)) {
      // Link-layer protocol

      // Send first header byte (message type id)
      tx_byte(trx_queue_head->id);
      crc16_init(&(trx_queue_head->crc));
      crc16_update(&(trx_queue_head->crc), trx_queue_head->id);
      crc16_update(&(trx_queue_head->crc), trx_queue_head->tx_remaining);
      // Send second header byte (message size)
      tx_byte(trx_queue_head->tx_remaining);
      PROCESS_YIELD();
      
      // Send message bytes
      while (trx_queue_head->tx_remaining > 0) {
	tx_byte(*(trx_queue_head->tx_buf));
	crc16_update(&(trx_queue_head->crc), *(trx_queue_head->tx_buf));
	trx_queue_head->tx_buf += 1;
	trx_queue_head->tx_remaining -= 1;
	PROCESS_YIELD();
      }
      
      // Send CRC footer bytes
      tx_byte(trx_queue_head->crc >> 8);
      tx_byte(trx_queue_head->crc & 0x00FF);
      
      // Wait for reply
      tx_dummy_byte();
      PROCESS_YIELD();
      while (get_rx_delay_remaining(trx_queue_head) > 0 &&
	     read_response_byte() == TYPE_RX_PROCESSING) {
	tx_dummy_byte();
	decrement_rx_delay_remaining(trx_queue_head);
	PROCESS_YIELD();
      }
      
      // Receive response header (first byte has already been received)
      crc16_init(&(trx_queue_head->crc));
      trx_queue_head->rx_buf[0] = read_response_byte();
      tx_dummy_byte(); // for the size byte
      if (trx_queue_head->rx_buf[0] == TYPE_CRC_FAILURE) {
	// CRC failure on response side, abort the transfer
	end_transfer(TRX_CRC_FAILURE);
	continue;
      }
      if (trx_queue_head->rx_buf[0] == TYPE_MESSAGE_TOO_LARGE) {
	// Message is too large for slave's receive buffer, abort the transfer
	end_transfer(TRX_MESSAGE_TOO_LARGE);
	continue;
      }
      
      crc16_update(&(trx_queue_head->crc), trx_queue_head->rx_buf[0]);
      trx_queue_head->rx_buf[1] = read_response_byte();
      tx_dummy_byte(); // for the first payload or footer byte
      if (trx_queue_head->rx_buf[1] > trx_queue_head->rx_remaining) {
	// rx_buf is too small for the response, abort the transfer
	end_transfer(TRX_RESPONSE_TOO_LARGE);
	continue;
      }
      trx_queue_head->rx_remaining = trx_queue_head->rx_buf[1];
      crc16_update(&(trx_queue_head->crc), trx_queue_head->rx_buf[1]);
      trx_queue_head->rx_buf += 2;
      
      PROCESS_YIELD();
      // Receive response payload
      while (trx_queue_head->rx_remaining > 0) {
	*(trx_queue_head->rx_buf) = read_response_byte();
	tx_dummy_byte();
	crc16_update(&(trx_queue_head->crc), *(trx_queue_head->rx_buf));
	trx_queue_head->rx_buf += 1;
	trx_queue_head->rx_remaining -= 1;
	PROCESS_YIELD();
      }
      
      // Receive response footer (first byte has already been received)
      uint16_t crc = read_response_byte() << 8;
      tx_dummy_byte();
      crc |= read_response_byte();
      if (! crc16_equal(crc, trx_queue_head->crc)) {
	// CRC failure, abort transfer
	end_transfer(TRX_RESPONSE_CRC_FAILURE);
	continue;
      }
    } else {
      // Simple transfer
      while (trx_queue_head->tx_remaining > 0) {
	tx_byte(*(trx_queue_head->tx_buf));
	trx_queue_head->tx_buf += 1;
	trx_queue_head->tx_remaining -= 1;
	PROCESS_YIELD();
	if (trx_queue_head->rx_remaining > 0) {
	  *(trx_queue_head->rx_buf) = read_response_byte();
	  trx_queue_head->rx_buf += 1;
	  trx_queue_head->rx_remaining -= 1;
	}
      }

      while (trx_queue_head->rx_remaining > 0) {
	tx_dummy_byte();
	PROCESS_YIELD();	
	*(trx_queue_head->rx_buf) = read_response_byte();
	trx_queue_head->rx_buf += 1;
	trx_queue_head->rx_remaining -= 1;
      }
    }
    // End current transfer
    end_transfer(TRX_COMPLETED_SUCCESSFULLY);
  }

  PROCESS_END();
}
