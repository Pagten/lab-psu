/*
 * timer.h
 *
 * Based on the timer implementation of Contiki (http://www.contiki-os.org).
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

#ifndef TIMER_H
#define TIMER_H

/**
 * @file timer.h
 * @author Pieter Agten <pieter.agten@gmail.com>
 * @date 14 jan 2014
 *
 * This file implements a timer. The implementation is based on the timer of
 * Contiki (http://www.contiki-os.org).
 */

#include <stdbool.h>

#include "core/clock.h"

typedef struct {
  clock_time_t start;
  clock_time_t delay;
} timer;

/**
 * Set a timer to expire after a given delay.
 *
 * The specified delay should be significantly shorter than CLOCK_TIME_MAX, to
 * avoid the clock overflowing before the timer expiration can be detected by
 * timer_expired().
 *
 * @param t     The timer to set
 * @param delay The delay in clock ticks after which the timer will expire
 */
void timer_set(timer* t, clock_time_t delay);

/**
 * Reset a timer.
 *
 * The timer will be reset with the same delay as was set previously using the
 * timer_set() function. The start of the period will be the time the timer
 * last expired.
 *
 * @param t  The timer to reset
 */
void timer_reset(timer* t);

/**
 * Restart a timer.
 *
 * The timer will be restarted with the same delay as was set previously using
 * the timer_set() function. The start of the period will be the current time.
 *
 * @param t  The timer to reset
 */
void timer_restart(timer* t);

/**
 * Check whether a timer has expired.
 *
 * @param t  The timer to check for expiration
 * @return true if the timer has expired, false otherwise.
 */
bool timer_expired(timer* t);

/**
 * Return the time remaining before the timer expires.
 *
 * @param t  The timer for which to return the remaining time
 * @return The number of clock ticks until the timer expires or 0 if the timer
 *         has already expired.
 */
clock_time_t timer_remaining(timer* t);


#endif
