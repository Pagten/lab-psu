/*
 * etimer.c
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
 * @file etimer.c
 * @author Pieter Agten <pieter.agten@gmail.com>
 * @date 04 Oct 2014
 */

#include "etimer.h"

#include "core/events.h"
#include "core/process.h"
#include "core/timer.h"

PROCESS(etimer_process);

static etimer* queue;

void init_etimer(void)
{
  queue = NULL;
  process_start(&etimer_process);
}


static void
remove_from_queue(etimer* t)
{
  etimer** pt = &queue;
  while (*pt != NULL && *pt != t) {
    pt = &((*pt)->next);
  }

  if (*pt == t) {
    // Remove from queue
    *pt = (*pt)->next;
  }
}

static inline
void insert(etimer* t, etimer** pt)
{
  t->next = *pt;
  *pt = t;
}


// Only call this function if you know that either t is not in the queue, or
// that t can stay in the same position or needs to move backward in the queue.
static void
insert_into_queue(etimer* t, clock_time_t now)
{
  clock_time_t remaining = etimer_remaining_at(t, now);

  // Find appropriate location in queue
  etimer** pt = &queue;
  while (*pt != NULL && remaining >= etimer_remaining_at(*pt, now)) {
    if (*pt == t) {
      // Remove from queue
      *pt = (*pt)->next;
    } else {
      pt = &((*pt)->next);
    }
  }

  // Insert at correct position in queue
  insert(t, pt);
}

void etimer_set(etimer* t, clock_time_t delay, process* p)
{
  remove_from_queue(t);
  timer_set(&(t->tmr), delay);
  t->p = p;
  insert_into_queue(t, t->tmr.start);
}

void etimer_reset(etimer* t)
{
  timer_reset(&(t->tmr));
  insert_into_queue(t, clock_get_time());
}

void etimer_restart(etimer* t)
{
  timer_restart(&(t->tmr));
  insert_into_queue(t, t->tmr.start);
}

bool etimer_expired_at(etimer* t, clock_time_t time)
{
  return timer_expired_at(&(t->tmr), time);
}

bool etimer_expired(etimer* t)
{
  return timer_expired(&(t->tmr));
}

clock_time_t etimer_remaining(etimer* t)
{
  return timer_remaining(&(t->tmr));
}

clock_time_t etimer_remaining_at(etimer* t, clock_time_t time)
{
  return timer_remaining_at(&(t->tmr), time);
}

PROCESS_THREAD(etimer_process)
{
  PROCESS_BEGIN();

  while(true) {
    PROCESS_YIELD();

    etimer* t = queue;
    clock_time_t now = clock_get_time();
    while (t != NULL && etimer_expired_at(t, now)) {
      if (t->p != NULL) {
	process_post_event(t->p, EVENT_TIMER_EXPIRED, (process_data_t)t);
      }
      t = t->next;
    }
    queue = t;
  }

  PROCESS_END();
}
