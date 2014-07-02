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
 * This file implements SPI slave communication.
 */

#include "core/process.h"

/**
 * Initialize the SPI slave module.
 */
void spis_init();

/**
 * Set the process to notify on incoming data
 */
void spis_set_rx_callback(process* p, process_event_t ev, uint8_t* buffer,
			  uint8_t hdr_size);

void spis_set_tx_buffer()


#endif
