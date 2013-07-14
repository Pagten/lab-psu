/*
 * spi.h
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

#ifndef SPI_H
#define SPI_H

/**
 * @file spi.h
 * @author Pieter Agten <pieter.agten@gmail.com>
 * @date 6 jun 2013
 *
 * This file implements SPI master communication.
 */

#define SPI_OK              0
#define SPI_QUEUE_FULL     -1
#define SPI_BUF_TOO_SMALL  -2

/**
 * Initializes the SPI master module.
 */
void spi_init();

/**
 * The type of the callback the SPI module calls when a data
 * packet was received in response to a transmitted data packet.
 */
typedef void (*spi_receive_callback)(void* buf_rx, void* data);

/**
 * Enumeration used for the SPI slave select callback, to indicate
 * whether to set or clear the slave's SS pin.
 */
typedef enum {SELECT_SLAVE, DESELECT_SLAVE} spi_slave_select_action;

/**
 * The type of the callback the SPI module calls in order to set
 * or clear the receiver's slave select pin.
 */
typedef void (*spi_slave_select_callback)(spi_slave_select_action action);

/**
 * Asynchronously transmit a data packet and return the received response
 * packet through a callback, as soon as possible. The transceive request
 * will be queued and will be handled as soon as the SPI hardware becomes
 * available. The caller must keep the given transmit buffer intact until
 * the receive callback is called. The receive buffer given in this callback
 * is not guaranteed to last after the callback returns, so the callback
 * should copy the received data to its own buffer.
 * 
 * @param buf_tx   The data to be sent
 * @param size_tx  The number of bytes to be sent
 * @param cb_ss    Callback to set or clear the receiver's SS pin
 * @param size_rx  The size of the packet that will be received
 * @param cb_rx    Callback to handle a received data packet
 * @param cb_rx_data Pointer to opaque data that will passed to the
 *                   cb_rx callback
 * @return SPI_OK when the request was queued succesfully, SPI_QUEUE_FULL
 *         if the transceive request queue is full or SPI_BUF_TOO_SMALL if
 *         the SPI receive buffer is too small to receive the packet.
 */
int8_t spi_transceive(void* buf_tx, uint8_t size_tx, spi_slave_select_callback cb_ss,
                      uint8_t size_rx, spi_receive_callback cb_rx, void *cb_rx_data);

#endif
