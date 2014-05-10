/*
 * gpio.h
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

#include "gpio.h"

#include <stdio.h>

#define NB_PORTS 3

static uint8_t pin_value[NB_PORTS];

void p_set_pins(port_ptr p, uint8_t mask)
{
  if ((uintptr_t)p >= NB_PORTS) {
    printf("Warning: unknown port '%p' specified in p_set_pins()", p);
    return;
  }

  pin_value[(uintptr_t)p] |= mask;
}


void p_clr_pins(port_ptr p, uint8_t mask)
{
  if ((uintptr_t)p >= NB_PORTS) {
    printf("Warning: unknown port '%p' specified in p_clr_pins()", p);
    return;
  }

  pin_value[(uintptr_t)p] &= ~mask;
}

uint8_t p_get_val(port_ptr p)
{
  if ((uintptr_t)p >= NB_PORTS) {
    printf("Warning: unknown port '%p' specified in p_get_val()", p);
    return 0;
  }

  return pin_value[(uintptr_t)p];
}


void p_set_val(port_ptr p, uint8_t value)
{
  if ((uintptr_t)p >= NB_PORTS) {
    printf("Warning: unknown port '%p' specified in p_get_val()", p);
    return;
  }

  pin_value[(uintptr_t)p] = value;
}
