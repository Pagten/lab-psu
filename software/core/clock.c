/*
 * clock.c
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
 * @file clock.c
 * @author Pieter Agten <pieter.agten@gmail.com>
 * @date 02 Feb 2014
 */

#include "clock.h"

#include <util/atomic.h>

#include "hal/interrupt.h"
#include "util/int.h"

#if TMR_SIZE(CLOCK_TMR) != 8
#error "The clock currently only supports 8-bit timers."
#endif


static volatile uint24_t clock_upper;

void clock_init()
{
  TMR_INIT(CLOCK_TMR);
  TMR_INTERRUPT_ENABLE(CLOCK_TMR, OVF);             // Enable overflow interrupt
  TMR_SET_MODE(CLOCK_TMR, NORMAL);                  // Set normal mode
  TMR_SET_CNTR(CLOCK_TMR, 0);                       // Set counter to 0
  TMR_SET_PRESCALER(CLOCK_TMR,CLOCK_TMR_PRESCALER); // Start timer

  clock_upper = 0;
}


static clock_time_t previous = 0;
clock_time_t clock_get_time()
{
  clock_time_t result;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    uint8_t cntr = TMR_GET_CNTR(CLOCK_TMR);
    if (TMR_IS_INTERRUPT_FLAG_SET(CLOCK_TMR, OVF) && cntr == 0) {
      // Timer has just overflowed and interrupt has not been handled yet
      result = ((clock_time_t)(clock_upper + 1) << 8);
    } else {
      result = ((clock_time_t)clock_upper << 8) | cntr;
    }
  }

  previous = result;
  return result;
}


// This interrupt takes 46 cycles when clock_upper is 24-bits wide
INTERRUPT(TMR_INTERRUPT_VECT(CLOCK_TMR, OVF))
{
  clock_upper += 1;
}
