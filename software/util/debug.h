/*
 * debug.h
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
 * @file debug.h
 * @author Pieter Agten (pieter.agten@gmail.com)
 * @date 23 May 2014
 */

#ifndef DEBUG_H
#define DEBUG_H

#include "hal/gpio.h"

#if DEBUG

#define LED0    B,0
#define LED1    D,7

inline static
void debug_init()
{
  SET_PIN_DIR_OUTPUT(LED0);
  SET_PIN_DIR_OUTPUT(LED1);
}

#define SET_DEBUG_LED(id)  SET_PIN(LED ## id)
#define CLR_DEBUG_LED(id)  CLR_PIN(LED ## id)
#define TGL_DEBUG_LED(id)  TGL_PIN(LED ## id)

#else 

#define SET_DEBUG_LED(id)
#define CLR_DEBUG_LED(id)
#define TGL_DEBUG_LED(id)


#endif


#endif
