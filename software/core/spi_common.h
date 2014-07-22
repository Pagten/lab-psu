/*
 * spi_common.h
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

#ifndef SPI_COMMON_H
#define SPI_COMMON_H

/**
 * @file spi_common.h
 * @author Pieter Agten <pieter.agten@gmail.com>
 * @date 3 Jul 2014
 *
 * This file contains some constants shared by the SPI master and slave.
 */

#define LLP_HEADER_LENGTH 2
#define LLP_FOOTER_LENGTH 2

#define MAX_RX_DELAY 15

#define MAX_RESPONSE_TYPE              0xFA
#define TYPE_RX_PROCESSING             0xFB
#define TYPE_ERR_NO_PROCESS_LISTENING  0xFC
#define TYPE_ERR_CRC_FAILURE           0xFD
#define TYPE_ERR_MESSAGE_TOO_LARGE     0xFE
#define TYPE_ERR_SLAVE_NOT_READY       0xFF

#endif
