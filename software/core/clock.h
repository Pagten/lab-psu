/*
 * clock.h
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

#ifndef CLOCK_H
#define CLOCK_H

#include "hal/timers.h"

#ifndef F_CPU
#warning "F_CPU not defined in clock.h!"
#endif

typedef uint16_t clock_time_t;

#define CLOCK_TMR            TIMER0
#define CLOCK_TMR_PRESCALER  1024

#define CLOCK_SEC    ((double)F_CPU/CLOCK_TMR_PRESCALER)  /** 1 second */
#define CLOCK_MSEC   (CLOCK_SEC/1000.0)                   /** 1 millisecond */
#define CLOCK_USEC   (CLOCK_SEC/1000000.0)                /** 1 microsecond */

#define CLOCK_TIME_MAX UINT16_MAX

/**
 * Initialize the clock.
 */
void clock_init(void);


/**
 * Return the current clock time.
 *
 * The clock ticks every 1024 CPU clock cycles and hence overflows every 
 * (1024 * 2^16)/F_CPU seconds. For F_CPU=16Mhz, the clock ticks every 64 us
 * and overflows every 4 seconds.
 */
clock_time_t clock_get_time(void);

#endif
