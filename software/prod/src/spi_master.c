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


void spi_m_init()
{
  
}

spi_m_trx_status spi_m_trx(uint8_t* tx_buf, size_t tx_buf_size,
                           volatile uint8_t *ss_port, uint8_t ss_mask,
                           uint8_t* rx_buf, size_t rx_buf_size,
                           spi_m_rx_callback rx_cb, void *rx_cb_data)
{
  
}
