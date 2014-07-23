/*
 * events.h
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

#ifndef EVENTS_H
#define EVENTS_H

/**
 * @file events.h
 * @author Pieter Agten <pieter.agten@gmail.com>
 * @date 11 Jul 2014
 *
 * This file contains the list of global events. Global events have an id 
 * greater than or equal to 128. User-defined events should have an id smaller
 * than 128.
 */

enum {
  // Process
  PROCESS_EVENT_INIT = 0x80,
  PROCESS_EVENT_CONTINUE,

  // SPI Master
  SPIM_TRX_COMPLETED_SUCCESSFULLY,
  SPIM_TRX_ERR_SLAVE_NOT_READY,
  SPIM_TRX_ERR_NO_RESPONSE,
  //  SPIM_TRX_ERR_CRC_FAILURE,
  SPIM_TRX_ERR_SLAVE,
  SPIM_TRX_ERR_RESPONSE_TOO_LARGE,
  SPIM_TRX_ERR_RESPONSE_CRC_FAILURE,

  // SPI Slave
  SPIS_MESSAGE_RECEIVED,
  SPIS_RESPONSE_TRANSMITTED,
  SPIS_RESPONSE_ERROR,
};

#endif
