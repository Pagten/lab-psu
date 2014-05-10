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

#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

typedef uint8_t* port_ptr;

#define PORTB_PTR  ((port_ptr)0)
#define PORTC_PTR  ((port_ptr)1)
#define PORTD_PTR  ((port_ptr)2)

void p_set_pins(port_ptr p, uint8_t mask);
void p_clr_pins(port_ptr p, uint8_t mask);

uint8_t p_get_val(port_ptr p);
void p_set_val(port_ptr p, uint8_t value);

#define P_SET_PINS(port, mask)              p_set_pins(port, mask)
#define P_CLR_PINS(port, mask)              p_clr_pins(port, mask)

#define P_GET_VAL(port)                     p_get_val(port)
#define P_SET_VAL(port, value)              p_set_val(port, value)

#define P_SET_PINS_DIR_OUTPUT(port, mask)
#define P_SET_PINS_DIR_INPUT(port, mask)


#endif
