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

#define HD44780_NB_CGRAM_ENTRIES 8

#define CGRAM_PATTERN(name, l0,l1,l2,l3,l4,l5,l6,l7) \
  hd44780_cgram_pattern name PROGMEM = { . pattern = {			\
    ((l0 & 0x1F) << 3) | ((l1 & 0x1F) >> 2),				\
    ((l1 & 0x1F) << 6) | ((l2 & 0x1F) << 1) | ((l3 & 0x1F) >> 4),	\
    ((l3 & 0x1F) << 4) | ((l4 & 0x1F) >> 1),				\
    ((l4 & 0x1F) << 7) | ((l5 & 0x1F) << 2) | ((l6 & 0x1F) >> 3),	\
    ((l6 & 0x1F) << 5) | ((l7 & 0x1F) }}


// Forward declaration:
typedef struct hd44780_lcd hd44780_lcd;

typedef struct {
  uint8_t pattern[5];
} hd44780_cgram_pattern;

struct cgram_entry {
  hd44780_cgram_pattern* p;
  uint8_t refcount;
};

typedef struct {
  struct cgram_entry entries[HD44780_NB_CGRAM_ENTRIES];
} hd44780_cgram;

typedef struct {
  hd44780_lcd* lcd;
  hd44780_cgram_pattern* p;
  char index;
} hd44780_cgram_char;


void hd44780_cgram_init(hd44780_cgram* cgram);

void hd44780_cgram_char_init(hd44780_cgram_char* c, hd44780_lcd* lcd,
			     hd44780_cgram_pattern* pattern);

char hd44780_cgram_char_acquire(hd44780_cgram_char* c);

char hd44780_cgram_char_get(hd44780_cgram_char* c);

void hd44780_cgram_char_release(hd44780_cgram_char* c);


#endif
