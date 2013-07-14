/*
 * spi.c
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
 * @file spi.c
 * @author Pieter Agten (pieter.agten@gmail.com)
 * @date 6 jun 2013
 */

#include "spi.h"

#ifndef SPI_RECEIVE_BUF_SIZE
#define SPI_RECEIVE_BUF_SIZE  64
#endif

static uint8_t receive_buf[SPI_RECEIVE_BUF_SIZE];

void spi_init()
{
  
}

int8_t spi_transceive(void* buf_tx, uint8_t size_tx, spi_slave_select_callback cb_ss,
                      uint8_t size_rx, spi_receive_callback cb_rx, void *cb_rx_data)
{
  
}
