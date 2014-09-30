/*
 * log_counters.h
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

/**
 * @file log_counters.h
 * @author Pieter Agten <pieter.agten@gmail.com>
 * @date 10 Aug 2014
 *
 * This file defines the default list of log counters. A custom, 
 * per-application list can be defined by creating a log_counters.h file
 * in the application's root directory.
 */


// Process
LOG_COUNTER_ON(EVENT_QUEUE_FULL)

// SPI Master
LOG_COUNTER_ON(SPIM_ERROR_RESPONSE)
LOG_COUNTER_ON(SPIM_RESPONSE_CRC_ERROR)
LOG_COUNTER_ON(SPIM_RESPONSE_TIMEOUT)
LOG_COUNTER_ON(SPIM_RESPONSE_TOO_LARGE)


// SPI Slave
LOG_COUNTER_ON(SPIS_TIMEOUT_WAITING_FOR_CALLBACK)
LOG_COUNTER_ON(SPIS_MESSAGE_TOO_LARGE)
LOG_COUNTER_ON(SPIS_CRC_FAILURE)
LOG_COUNTER_ON(SPIS_TRX_COMPLETED)
