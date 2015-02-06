/*
 * pgmspace.h
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

#ifndef PGMSPACE_H
#define PGMSPACE_H

/**
 * @file pgmspace.h
 * @author Pieter Agten (pieter.agten@gmail.com)
 * @date 17 Oct 2014
 */

#include <string.h>

#define PROGMEM
#define PGM_P const char *

#define strncpy_P strncpy
#define pgm_read_word(addr)  (*(addr))


#endif
