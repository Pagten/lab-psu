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
 * @date 17 Dec 2013
 */

#include <avr/interrupt.h>

#define INTERRUPT(vect,...)      ISR(vect,__VA_ARGS__)  
#define INTERRUPT_ALIAS(target)  ISR_ALIASOF(target)

#define ENABLE_INTERRUPTS()  sei()
#define DISABLE_INTERRUPTS() cli()

#endif
