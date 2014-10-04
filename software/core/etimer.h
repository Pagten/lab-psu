/*
 * etimer.h
 *
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

#ifndef ETIMER_H
#define ETIMER_H

/**
 * @file etimer.h
 * @author Pieter Agten <pieter.agten@gmail.com>
 * @date 04 Oct 2015
 *
 * This is the event timer: a timer that sends an event to a process when it
 * expires.
 */

#include <stdbool.h>
#include <stddef.h>

#include "core/clock.h"
#include "core/timer.h"
#include "core/process.h"

struct etimer {
  timer tmr;
  process* p;
  struct etimer* next;
};
typedef struct etimer etimer;

/**
 * Initialize the even timer module.
 *
 * Modules that should be initialized first:
 *  * clock
 */
void init_etimer(void);


/**
 * Set an event timer to expire after a given delay.
 *
 * The specified delay should be significantly shorter than CLOCK_TIME_MAX, to
 * avoid the clock overflowing before the timer expiration can be detected.
 *
 * @param t     The event timer to set
 * @param delay The delay in clock ticks after which the timer will expire
 * @param p     The process to notify with the EVENT_TIMER_EXPIRED event when
 *              the event timer expires. The data of the event will be the
 *              pointer to the given timer t.
 */
void etimer_set(etimer* t, clock_time_t delay, process* p);


/**
 * Reset an event timer.
 *
 * The event timer will be reset with the same delay and process as was set
 * previously using the etimer_set() function. The start of the new period
 * will be the time the timer last expired.
 *
 * @param t  The event timer to reset
 */
void etimer_reset(etimer* t);

/**
 * Restart an event timer.
 *
 * The event timer will be restarted with the same delay and process as was set
 * previously using the etimer_set() function. The start of the new period will
 * be the current time.
 *
 * @param t  The event timer to reset
 */
void etimer_restart(etimer* t);

/**
 * Check whether an event timer is expired at a certain moment in time.
 *
 * @param t    The event timer to check for expiration
 * @param time The time at which to check for expiration
 * @return true if the event timer will be expired at the given time, false
 *         otherwise.
 */
bool etimer_expired_at(etimer* t, clock_time_t time);


/**
 * Check whether an event timer has expired.
 *
 * @param t  The event timer to check for expiration
 * @return true if the event timer has expired, false otherwise.
 */
bool etimer_expired(etimer* t);


/**
 * Return the time between a given moment and the moment the event timer
 * expires.
 *
 * @param t    The event timer for which to return the remaining time
 * @param time The time from which to count the remaining time
 * @return The number of clock ticks until the event timer expires from the
 *         given moment in time, or 0 if the timer has already expired at the
 *         given moment.
 */
clock_time_t etimer_remaining_at(etimer* t, clock_time_t time);


/**
 * Return the time remaining before an event timer expires.
 *
 * @param t  The event timer for which to return the remaining time
 * @return The number of clock ticks until the event timer expires or 0 if it
 *         has already expired.
 */
clock_time_t etimer_remaining(etimer* t);


#endif
