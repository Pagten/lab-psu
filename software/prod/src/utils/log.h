/*
 * log.h
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
 * @file log.h
 * @author Pieter Agten <pieter.agten@gmail.com>
 * @date 14 jul 2013
 *
 * Logging functions
 */

#ifndef LOG_H
#define LOG_H

#include <stdio.h>

#ifdef LOGGING_ENABLED
#define IFLE(c) (x)
#else
#define IFLE(c)
#endif


#define LOG_WARN(msg)  IFLE(puts(msg);)


#endif
