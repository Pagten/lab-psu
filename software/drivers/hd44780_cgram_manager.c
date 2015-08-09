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

static void decode_pattern(hd44780_cgram_char* c, uint8_t dst[8])
{
  uint8_t line;
  line = pgm_read_byte(&(c->p.pattern[0]));
  dst[0] = line >> 3;
  
  dst[1] = (line << 2) & 0x1F;
  line = pgm_read_byte(&(c->p.pattern[1]));
  dst[1] |= line >> 6;

  dst[2] = (line >> 1) & 0x1F;

  dst[3] = (line << 4) & 0x1F
  line = pgm_read_byte(&(c->p.pattern[2]));
  dst[3] |= line >> 4;
 
  dst[4] = (line << 1) & 0x1F;
  line = pgm_read_byte(&(c->p.pattern[3]));
  dst[4] |= line >> 7;

  dst[5] = line >> 2;

  dst[6] = (line << 3) & 0x1F;
  line = pgm_read_byte(&(c->p.pattern[4]));
  dst[6] |= line >> 5;

  dst[7] = line & 0x1F;
}

static void
upload_to_cgram(hd44780_cgram_char* c)
{
  uint8_t pattern[8];
  decode_pattern(pattern);

  hd44780_cgram_write(c->lcd, c->index, pattern);
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
      if (free == INDEX_NOT_IN_USE && entries[i].refcount == 0) {
	free = i;
      }
      i += 1;
    }

    if (i < NB_CGRAM_ENTRIES) {
      // Pattern already present, so just increase the refcount
      if (entries[i].refcount < UINT8_MAX) {
	c->index = i;
	entries[i].refcount += 1;
      } else {
	// Refcount overflow!
	// TODO: LOG!
      }
    } else if (free < NB_CGRAM_ENTRIES) {
      // Pattern not yet present, so upload the pattern to a free spot
      c->index = free;
      entries[i].refcount = 1;
      upload_to_cgram(c);
    } else {
      // Pattern not yet present and no free CGRAM spots left...
      // TODO: LOG!
    }
  }

  return c->index;
}

char hd44780_cgram_char_get(hd44780_cgram_char* c)
{
  return c->index;
}

void hd44780_cgram_char_release(hd44780_cgram_char* c)
{
  c->lcd->cgram->entries[c->index].refcount -= 1;
  c->index = INDEX_NOT_IN_USE;
}

