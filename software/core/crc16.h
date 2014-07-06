/*
 * crc16.h
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

#ifndef CRC16_H
#define CRC16_H

/**
 * @file crc16.h
 * @author Pieter Agten (pieter.agten@gmail.com)
 * @date 6 Jul 2014
 */

#include <stdint.h>
#include <stdbool.h>
#include <util/crc16.h>


typedef uint16_t crc16;

static inline 
void crc16_init(crc16* crc)
{
  *crc = 0xFFFF;
}

static inline
void crc16_update(crc16* crc, uint8_t val)
{
  *crc = crc16_update(*crc, val);
}

static inline
bool crc16_equal(crc16* crc0, crc16* crc1)
{
  return *crc0 == *crc1;
}

#endif
