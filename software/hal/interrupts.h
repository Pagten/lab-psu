/*
 * interrupt.h
 *
 * Copyright 2013 Pieter Agten
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

#ifndef INTERRUPT_H
#define INTERRUPT_H

/**
 * @file interrupt.h
 * @author Pieter Agten <pieter.agten@gmail.com>
 * @date 17 dec 2013
 */

#include <avr/interrupt.h>

#define DEF_CONST(b_const) DEF_CONST_##b_const
#define DEF_CONST_true const
#define DEF_CONST_false

#define _ISR_ALIAS(defined_isr, new_isr)  

#endif
