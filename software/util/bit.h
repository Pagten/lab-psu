/*
 * bit.h
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

/**
 * @file bit.h
 * @author Pieter Agten <pieter.agten@gmail.com>
 * @date 14 Jul 2013
 *
 * Bit-fiddling related helper functions
 */

#ifndef BIT_H
#define BIT_H

#define _BV(bit)   (1 << (bit))

static inline
uint8_t bv8(uint8_t v) {
  return 1 << v; //TODO: check whether this gets optimized
}


#endif
