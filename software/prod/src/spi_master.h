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
  SPI_M_OK,
  SPI_M_QUEUE_FULL,
} spi_m_trx_status;


/**
 * Initializes the SPI master module.
 */
void spi_m_init();

/**
 * The type of the callback the SPI module calls when a data
 * packet was received in response to a transmitted data packet.
 */
typedef void (*spi_m_rx_callback)(size_t rx_size, void* data);


/**
 * Asynchronously transmit a data packet and return the received response
 * packet through a callback, as soon as possible. The transceive request will
 * be queued and will be handled as soon as the SPI hardware becomes available.
 * The caller must keep the given transmit buffer intact until the receive
 * callback is called. The specified slave select pin(s) will be pulled low
 * during the data transfer. The received response will be stored in the given
 * receive buffer. The callback will be called when the response has been
 * received, with the number of bytes received (up to the specified receive
 * buffer size) as the first argument and the given rx_cb_data opaque pointer
 * as the second argument. 
 * 
 * @param tx_buf       The data to be sent
 * @param tx_buf_size  The number of bytes to be sent
 * @param ss_port      The slave select port
 * @param ss_mask      The slave select pin mask
 * @param rx_buf       The receive buffer, should be large enough to hold the
 *                     response
 * @param rx_buf_size  The size of the receive buffer
 * @param rx_cb        Callback for handling a received data packet
 * @param rx_cb_data   Pointer to be passed on to the rx_cb callback
 * @return SPI_M_OK when the request was queued succesfully or SPI_M_QUEUE_FULL
 *         if the transceive request queue is full.
 */
spi_m_trx_status spi_m_trx(uint8_t* tx_buf, size_t tx_buf_size,
                           volatile uint8_t *ss_port, uint8_t ss_mask,
                           uint8_t* rx_buf, size_t rx_buf_size,
                           spi_m_rx_callback rx_cb, void *rx_cb_data)


#endif
