/*
 * hd44780_cgram_manager.h
 *
 * Copyright 2015 Pieter Agten
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

#ifndef HD44780_CGRAM_MANAGER_H
#define HD44780_CGRAM_MANAGER_H

/**
 * @file hd44780_cgram_manager.h
 * @author Pieter Agten (pieter.agten@gmail.com)
 * @date 07 Aug 2015
 *
 */


typedef struct {
  uint8_t pattern[8];
} hd44780_cgram_pattern;

typedef struct {
  hd44780_cgram_pattern* p;
  uint8_t refcount;
} hd44780_cgram_pattern_ref;

typedef struct {
  hd44780_lcd* lcd;
  hd44780_cgram_pattern* c;
  uint8_t index;
} hd44780_cgram_pattern_lock;

void hd44780_cgram_acquire(cgram_lock* l);


void hd44780_cgram_release(cgram_lock* l);


#endif
