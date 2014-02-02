/*
 * pp_magic.h
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
 * @file pp_magic.h
 * @author Pieter Agten <pieter.agten@gmail.com>
 * @date 01 feb 2014
 *
 * Preprocessor magic
 */

// Returns the number of arguments in a __VA_ARGS__ (max 9 args)
#define NARG_(_9, _8, _7, _6, _5, _4, _3, _2, _1, n, ...) n
#define NARG(...) NARG_(__VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

#define _PASTE2(a, b) __PASTE2(a, b)
#define __PASTE2(a, b) a ## b

// Concatenate with macro-expansion
#define _CAT2(a,b)               a ## b
#define _CAT3(a,b,c)             a ## b ## c
#define _CAT4(a,b,c,d)           a ## b ## c ## d
#define _CAT5(a,b,c,d,e)         a ## b ## c ## d ## e
#define _CAT6(a,b,c,d,e,f)       a ## b ## c ## d ## e ## f
#define _CAT7(a,b,c,d,e,f,g)     a ## b ## c ## d ## e ## f ## g
#define _CAT8(a,b,c,d,e,f,g,h)   a ## b ## c ## d ## e ## f ## g ## h
#define _CAT9(a,b,c,d,e,f,g,h,i) a ## b ## c ## d ## e ## f ## g ## h ## i
#define CAT(...) _PASTE2(_CAT, NARG(__VA_ARGS__)(__VA_ARGS__))
