/*
 * io_monitor.h
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

#ifndef IO_MONITOR_H
#define IO_MONITOR_H

/**
 * @file io_monitor.h
 * @author Pieter Agten <pieter.agten@gmail.com>
 * @date 07 Apr 2014
 *
 * The I/O monitor detects I/O port changes and can send events to notify
 * processes about such changes. The monitor works by periodically polling the
 * I/O ports, thereby automatically debouncing any connected mechanical
 * switches.
 *
 * Note: This module uses the same hardware timer as the clock module.
 */

#include "hal/gpio.h"
#include "process.h"

typedef enum {
  IOMON_PORTB = 0,
  IOMON_PORTC = 1,
  IOMON_PORTD = 2,
} iomon_port;

typedef struct iomon_event {
  iomon_port port;
  uint8_t mask;
  process* process;
  process_event_t event;
  struct iomon_event* next;
} iomon_event;

typedef enum {
  IOMON_EVENT_INIT_OK,
  IOMON_EVENT_INIT_INVALID,
} iomon_event_init_status;

typedef enum {
  IOMON_EVENT_ENABLE_OK,
  IOMON_EVENT_ALREADY_ENABLED,
} iomon_event_enable_status;

typedef enum {
  IOMON_EVENT_DISABLE_OK,
  IOMON_EVENT_ALREADY_DISABLED,
} iomon_event_disable_status;

// To extract the debounced and toggled states from the process event data
#define DEBOUNCED(data)   UNPACK_PROCESS_DATA0(data)
#define TOGGLED(data)     UNPACK_PROCESS_DATA1(data)

// Non-portable, depends on specific port addresses
#define PORT_PTR_TO_IOMON_PORT(pptr)				\
  ((uint8_t)((uintptr_t)(pptr) & (uintptr_t)0x03) ^ 0x01)

/**
 * Initialize the I/O monitor module.
 *
 * Module that must be initialized first:
 *  - clock
 *  - process
 */
void iomon_init();


/**
 * Initialize an I/O monitor event.
 * 
 * An I/O monitoring event must be initialized before it can be used as an
 * argument to any of the other function in this file. It is allowed to
 * re-initialize an I/O monitor that has been enabled.
 * 
 * @param e     The I/O monitor event to initialize.
 * @param port  The I/O port to monitor.
 * @param mask  Bit mask to indicate which pins of the specified port should be
 *              monitored.
 * @param p     The process to notify when the specified I/O pins change.
 * @param ev    The event to send to the process when the specified I/O pins
 *              change.
 * @param IOMON_EVENT_INIT_OK if the I/O monitoring event was initialized
 *        successfully, or IOMON_EVENT_INIT_INVALID if the specified port to
 *        monitor was invalid.
 */
iomon_event_init_status
iomon_event_init(iomon_event* e, iomon_port port, uint8_t mask,
		 process* p, process_event_t ev);


/**
 * Enable an I/O monitor event.
 *
 * After calling this function, the I/O monitor event's process will be
 * notified whenever a state change of the specified I/O pins is detected. The
 * data sent with the event will be the debounced port value packed together
 * with a bit vector indicating which pins changed.
 *
 * @param e  The I/O monitor event to enable.
 * @return IOMON_EVENT_ENABLE_OK when the event was enabled successfully, or
 *         IOMON_EVENT_ALREADY_ENABLED when the event already was enabled.
 */
iomon_event_enable_status iomon_event_enable(iomon_event* e);


/**
 * Disable an I/O monitor event.
 *
 * After calling this function, the I/O monitor event's process will no
 * longer be notified when a state change of the specified I/O pins is
 * detected.
 *
 * @param e  The I/O monitor event to disable.
 * @return IOMON_EVENT_DISABLE_OK when the event was disabled successfully, or
 *         IOMON_EVENT_ALREADY_DISABLED when the event already was disabled. 
 */
iomon_event_disable_status iomon_event_disable(iomon_event* e);


#endif
