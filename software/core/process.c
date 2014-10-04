/*
 * process.c
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
 * @file process.c
 * @author Pieter Agten <pieter.agten@gmail.com>
 * @date 08 Feb 2014
 *
 * This file implements processes, based on protothreads. It is inspired by the
 * processes implementation of Contiki.
 *
 * @see http://dunkels.com/adam/pt
 * @see http://www.contiki-os.org
 */

#include "process.h"

#include <stdlib.h>
#include <util/atomic.h>
#include "core/clock.h"
#include "util/log.h"

// Should preferably be a power of 2
#define PROCESS_CONF_EVENT_QUEUE_SIZE 16

struct event {
  process* p;
  process_event_t ev;
  process_data_t data;
};

struct event_queue {
  struct event queue[PROCESS_CONF_EVENT_QUEUE_SIZE];
  volatile uint8_t count;
  uint8_t first;
};

static struct event_queue queues[NB_PROCESS_EVENT_PRIORITIES];

void process_init(void)
{
  int i;
  for (i = 0; i < NB_PROCESS_EVENT_PRIORITIES; ++i) {
    queues[i].count = 0;
    queues[i].first = 0;
  }
}


void process_start(process* p)
{
  // Initialize the protothread
  PT_INIT(&p->pt);

  // Synchronously send INIT event
  p->thread(p, PROCESS_EVENT_INIT, PROCESS_DATA_NULL);
}

// Can be called from an interrupt:
static inline process_post_event_status
post_event(process* p, struct event_queue* q, process_event_t ev,
	   process_data_t data)
{
  uint8_t i;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    if (q->count == PROCESS_CONF_EVENT_QUEUE_SIZE) {
      LOG_COUNTER_INC(EVENT_QUEUE_FULL);
      return PROCESS_POST_EVENT_QUEUE_FULL;
    }

    i = (q->first + q->count) % PROCESS_CONF_EVENT_QUEUE_SIZE;
    q->count += 1;
  }
  q->queue[i].p = p;
  q->queue[i].ev = ev;
  q->queue[i].data = data;
  
  return PROCESS_POST_EVENT_OK;
}

process_post_event_status
process_post_event(process* p, process_event_t ev, process_data_t data)
{
  return post_event(p, &queues[0], ev, data);
}

process_post_event_status
process_post_priority_event(process* p, process_event_t ev, process_data_t data,
		   process_event_priority pri)
{
  if (pri >= NB_PROCESS_EVENT_PRIORITIES) {
    return PROCESS_POST_EVENT_INVALID_PRIORITY;
  }
  return post_event(p, &queues[pri], ev, data);
}



void process_execute(void)
{
  // Find first non-empty queue
  int qi = 0;
  while (qi < NB_PROCESS_EVENT_PRIORITIES && queues[qi].count == 0) {
    qi += 1;
  }
  if (qi == NB_PROCESS_EVENT_PRIORITIES) {
    // No events to process
    return;
  }

  // Process one event from the selected queue
  struct event e;
  struct event_queue* q = &queues[qi];
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    e = q->queue[q->first];
    q->count -= 1;
    q->first = (q->first + 1) % PROCESS_CONF_EVENT_QUEUE_SIZE;
  }
#ifdef PROCESS_STATS
  clock_time_t clock_before = clock_get_time();
#endif
  e.p->thread(e.p, e.ev, e.data);
#ifdef PROCESS_STATS
  clock_time_t duration = clock_get_time() - clock_before;
  e.p->time += duration;
#endif
}


clock_time_t process_get_time(process* p)
{
#ifdef PROCESS_STATS
  return p->time;
#else
  return 0;
#endif
}
