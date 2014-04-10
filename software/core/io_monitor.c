/*
 * io_monitor.c
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
 * @file io_monitor.c
 * @author Pieter Agten <pieter.agten@gmail.com>
 * @date 07 Apr 2014
 */

#include "io_monitor.h"

#include <stdlib.h>

#include "hal/timers.h"
#include "hal/interrupt.h"
#include "clock.h"

#define NB_PORTS 3

PROCESS(event_dispatcher);

#define INT_TO_PORT_CHANGED_EVENT(i) ((process_event_t)i)

static iomon_event* iomon_event_list_head[NB_PORTS];
static uint8_t port_mask[NB_PORTS];

void iomon_init()
{
  for (uint8_t p = 0; p < NB_PORTS; ++p) {
    iomon_event_list_head[p] = NULL;
    port_mask[p] = 0x00;
  }
  TMR_INTERRUPT_ENABLE(CLOCK_TMR, OCA); // Enable OCA interrupt
}

iomon_event_init_status
iomon_event_init(iomon_event* e, iomon_port port, uint8_t mask,
		 process* p, process_event_t ev)
{
  if (port >= NB_PORTS) {
    return IOMON_EVENT_INIT_INVALID;
  }

  e->port = port;
  e->mask = mask;
  e->process = p;
  e->event = ev;
  return IOMON_EVENT_INIT_OK;
}


iomon_event_enable_status iomon_event_enable(iomon_event* e)
{
  iomon_event** ev = &iomon_event_list_head[e->port];
  while (*ev != NULL && *ev != e) {
    ev = &((*ev)->next);
  }

  if (*ev == e) {
    return IOMON_EVENT_ALREADY_ENABLED;
  }

  *ev = e;
  e->next = NULL;
  //  ATOMIC_BLOCK(ATOMIC_RESTORE_STATE) {
    //TODO: check asm to see if this block is needed
    port_mask[e->port] |= e->mask;
    //}
 
  return IOMON_EVENT_ENABLE_OK;
}


iomon_event_disable_status iomon_event_disable(iomon_event* e)
{
  bool found = false;
  uint8_t new_port_mask = 0x00;
  iomon_event** ev = &iomon_event_list_head[e->port];
  while (*ev != NULL) {
    if (*ev == e) {
      // Remove from list
      *ev = e->next;
      found = true;
    } else {
      new_port_mask |= (*ev)->mask;
    
      ev = &((*ev)->next);
    }
  }

  if (! found) {
    return IOMON_EVENT_ALREADY_DISABLED;
  }

  //  ATOMIC_BLOCK(ATOMIC_RESTORE_STATE) {
    //TODO: check asm to see if this block is needed
    port_mask[e->port] = new_port_mask;
    //}
  return IOMON_EVENT_DISABLE_OK;
}



INTERRUPT(TMR_INTERRUPT_VECT(CLOCK_TMR, OCA))
{
  // Debouncing based on 2-bit vertical counter: we require 4 identical samples
  // before we signal a pin change
  static uint8_t debounced[NB_PORTS];
  static uint8_t counter0[NB_PORTS];
  static uint8_t counter1[NB_PORTS];

  //TODO: check if gcc is smart enough to transform these into direct reads:
  uint8_t sample[NB_PORTS] = {
    P_GET_VAL(&PORTB) & port_mask[0],
    P_GET_VAL(&PORTC) & port_mask[1],
    P_GET_VAL(&PORTD) & port_mask[2]
  };


  for (uint8_t p = 0; p < NB_PORTS; ++p) {
    uint8_t delta = debounced[p] ^ sample[p];
    counter1[p] = (counter1[p] ^ counter0[p]) & delta;        
    counter0[p] = ~counter0[p] & delta;
    uint8_t toggled = delta & ~(counter1[p] | counter0[p]);
    if (toggled) {
      debounced[p] ^= toggled; 
      process_post_event(&event_dispatcher, INT_TO_PORT_CHANGED_EVENT(p),
			 PACK_PROCESS_DATA(debounced[p],toggled));
    }
  }
}


PROCESS_THREAD(event_dispatcher)
{
  PROCESS_BEGIN();

  while (true) {
    PROCESS_WAIT_EVENT();

    if (ev < NB_PORTS) {
      uint8_t toggled = UNPACK_PROCESS_DATA1(data);

      iomon_event* e = iomon_event_list_head[ev];
      while (e != NULL) {
	if (toggled & e->mask) {
	  process_post_event(e->process, e->event, data);
	}
	e = e->next;
      }
    }
  }

  PROCESS_END();
}
