/*
 * eeprom.h
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
 * @file eeprom.c
 * @author Pieter Agten (pieter.agten@gmail.com)
 * @date 11 Oct 2014
 */


#include "eeprom.h"

#include <avr/eeprom.h>

#include "core/crc16.h"

void
eeprom_read_block_crc(void* dst, const void* src, size_t size, crc16* crc)
{
  uint8_t* _dst = dst;
  const uint8_t* _src = src;
  while (size > 0) {
    *_dst = eeprom_read_byte(_src);
    crc16_update(crc, *_dst);
    _dst += 1;
    _src += 1;
    size -= 1;
  }
}

void
eeprom_update_block_crc(const void* src, void* dst, size_t size, crc16* crc)
{
  const uint8_t* _src = src;
  uint8_t* _dst = dst;
  while (size > 0) {
    eeprom_update_byte(_dst, *_src);
    crc16_update(crc, *_src);
    _src += 1;
    _dst += 1;
    size -= 1;
  }
}
