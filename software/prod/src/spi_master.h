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

typedef enum
{
  SPIM_OK,
  SPIM_QUEUE_FULL,
} spim_trx_status;

typedef enum
{
  SPIM_RX_DONE,
  SPIM_TX_DONE,
  SPIM_ERR_SCHED_FAILED,
} spim_cb_status


extern uint8_t spim_receive_buf;

/**
 * Initializes the SPI master module.
 */
void spim_init();

/**
 * The type of the callback the SPI module calls when a data
 * packet was received in response to a transmitted data packet.
 */
typedef size_t (*spim_trx_callback)(spim_cb_status status, void *rx_cb_data);


/**
 * Asynchronously transmit a data buffer and return the received response
 * through a callback. The transceive request will be queued and will be
 * handled as soon as the SPI hardware becomes available. The specified slave
 * select pin(s) will be pulled low during the data transfer and pulled high
 * afterwards. The caller must keep the given transmit/receive buffer intact
 * until all bytes are sent/received. The rx_cb_callback will be called when
 * the transfer is completed. The argument passed to the callback is the opaque
 * rx_cb_data pointer. 
 *
 * A minimum time of trx_delay scheduler timer ticks is waited before
 * sending/receiving each byte, to give the slave device time to prepare the
 * next byte to be sent.
 * 
 * @param tx_buf       The data to be sent
 * @param tx_size      The number of bytes to be sent
 * @param trx_delay    Scheduler timer ticks to wait between each byte
 * @param ss_port      The slave select port
 * @param ss_mask      The slave select pin mask
 * @param rx_buf       The buffer to place the received bytes into
 * @param rx_size      The number of bytes to receive before calling cb_rx
 * @param trx_cb       Callback for handling a received data packet
 * @param trx_cb_data  Pointer to be passed on to the rx_cb callback
 * @return SPIM_OK when the request was queued succesfully or SPIM_QUEUE_FULL
 *         if the transceive request queue is full.
 */
spim_trx_status spim_trx(uint8_t *tx_buf, size_t tx_size, ticks_t trx_delay,
                         volatile uint8_t *ss_port, uint8_t ss_mask,
                         uint8_t *rx_buf, size_t rx_size,
                         spim_trx_callback trx_cb, void *trx_cb_data)


#endif
