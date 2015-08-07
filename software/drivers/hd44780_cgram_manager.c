/*
 * hd44780_cgram_manager.c
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

/**
 * @file hd44780_cgram_manager.c
 * @author Pieter Agten (pieter.agten@gmail.com)
 * @date 07 Aug 2015
 */

#define INDEX_NOT_IN_USE 0xFF

void hd44780_cgram_init(hd44780_cgram* cgram)
{
  for (uint8_t i = 0; i < NB_CGRAM_ENTRIES; ++i) {
    cgram->entries[i] = 0;
  }
}

void hd44780_cgram_char_init(hd44780_cgram_char* c, hd44780_lcd* lcd,
			     hd44780_cgram_pattern* pattern)
{
  c->lcd = lcd;
  c->p = pattern;
  c->index = INDEX_NOT_IN_USE;

}

static void
upload_to_cgram(hd44780_cgram_char* c)
{
  // TODO: upload to CGRAM and then restore previous DDRAM address
  
}

char hd44780_cgram_char_acquire(hd44780_cgram_char* c)
{
  if (c->index == INDEX_NOT_IN_USE) {
    // Char not yet acquired, so first check if pattern is already present in
    // the LCD's CGRAM
    char i = 0;
    char free = INDEX_NOT_IN_USE;
    struct cgram_entry entries[] = c->lcd->cgram->entries;
    while (i < NB_CGRAM_ENTRIES && entries[i].p != pattern) {
      if (free == INDEX_NOT_IN_USE && entries[i].p == NULL) {
	free = i;
      }
      i += 1;
    }

    if (i < NB_CGRAM_ENTRIES) {
      // Pattern already present, so just increase the refcount
      if (entries[i].refcount < UINT8_MAX) {
	entries[i].refcount += 1;
	c->index = i;
      } else {
	// Refcount overflow!
	// TODO: LOG!
      }
    } else if (free < NB_CGRAM_ENTRIES) {
      // Pattern not yet present, so upload the pattern to a free spot
      upload_to_cgram(c);
      entries[i].refcount = 1;
      c->index = free;
    } else {
      // Pattern not yet present and no free CGRAM spots left...
      // TODO: LOG!
    }
  }

  return c->index;
}

char hd44780_cgram_char_get(hd44780_cgram_char* c);

void hd44780_cgram_char_release(hd44780_cgram_char* c);
