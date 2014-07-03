/*
 * spi_slave.h
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

#ifndef SPI_SLAVE_H
#define SPI_SLAVE_H

/**
 * @file spi_slave.h
 * @author Pieter Agten <pieter.agten@gmail.com>
 * @date 30 Jun 2014
 *
 * This file provides SPI slave communication using a simple link layer
 * protocol designed for request-response type messages. The protocol allows
 * the slave device to specify the length of its response. It also allows the
 * slave device to delay its response for up to 16 bytes after the master has
 * sent its message, which can be useful if the slave needs some time to
 * generate its response. The detailed description of the protocol can be
 * found in spi_master.h.
 */

#include "core/process.h"

/**
 * Initialize the SPI slave module.
 */
void spis_init();

/**
 * Set the process to notify on incoming data
 *
 * @param p  The process to notify when a message from the SPI master was
 *           received
 */
void spis_set_rx_callback(process* p);

/**
 * Send a response in reply to a message from the SPI master. This function
 * should be called within 16 SPI clock periods after the process set using
 * the spis_set_rx_callback() function was notified of an incoming message.
 *
 * @param r  The response to send
 * @return TODO
 */
spis_send_response_status
spis_send_response(spis_response* r);


#endif
