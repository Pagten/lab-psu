/*
 * mock_timer.c
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

#include "mock_timer.h"

#include <stdio.h>
#include <stdlib.h>


static void fire_interrupts(mock_timer* tmr);

void mock_timer_init(mock_timer* tmr)
{
  tmr->cntr = 0;
  tmr->ocra = 0;
  tmr->ocrb = 0;
  tmr->mode = M_NORMAL;
  tmr->oca_intr_enabled = false;
  tmr->ocb_intr_enabled = false;
  tmr->ovf_intr_enabled = false;
}

void mock_timer_tick(mock_timer* tmr)
{
  if (! tmr->clock_enabled) {
    fputs("warning: mock timer tick while disabled", stderr);
  }

  switch (tmr->mode) {
  case M_NORMAL: 
    tmr->cntr += 1;
    if (tmr->cntr == (1 << tmr->nb_bits)) {
      tmr->cntr = 0;
    }
    fire_interrupts(tmr);
    break;
  default:
    fputs("error: mock timer mode not implemented yet", stderr);
    abort();
  }
}

void mock_timer_channel_disconnect(mock_timer* tmr, tmr_channel ch)
{ }

void mock_timer_set_intr_enabled(mock_timer* tmr, tmr_interrupt intr, bool val)
{
  switch (intr) {
  case INTR_OCA:
    tmr->oca_intr_enabled = val;
    break;
  case INTR_OCB:
    tmr->ocb_intr_enabled = val;
    break;
  case INTR_OVF:
    tmr->ovf_intr_enabled = val;
    break;
  default:
    fputs("error: unknown timer interrupt", stderr);
    abort();
  }
}
void mock_timer_set_mode(mock_timer* tmr, tmr_mode mode)
{
  if (mode != M_NORMAL) {
    fputs("error: mock timer mode not implemented yet", stderr);
    abort();
  }

  tmr->mode = mode;
}

void mock_timer_set_clock(mock_timer* tmr, tmr_clock_src cs)
{
  tmr->clock_enabled = (cs != CS_DISABLED);
}

void mock_timer_set_cntr(mock_timer* tmr, uint16_t val)
{
  tmr->cntr = val;
}

uint8_t mock_timer_get_cntr8(mock_timer* tmr)
{
  return (uint8_t)tmr->cntr;
}

uint16_t mock_timer_get_cntr16(mock_timer* tmr)
{
  return tmr->cntr;
}


static void fire_interrupts(mock_timer* tmr)
{
  if (tmr->oca_intr_enabled && 
      tmr->cntr == tmr->ocra) {
    tmr->oca_vect();
  }
  if (tmr->ocb_intr_enabled && 
      tmr->cntr == tmr->ocrb) {
    tmr->ocb_vect();
  }
  if (tmr->ovf_intr_enabled && 
      tmr->cntr == 0) {
    tmr->ovf_vect();
  }
}
