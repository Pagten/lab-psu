/*
 * hd44780_cgram_patterns.c
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
 * @file hd44780_cgram_patterns.c
 * @author Pieter Agten (pieter.agten@gmail.com)
 * @date 09 Aug 2015
 */


CGRAM_PATTERN(HD44780_INVERTED_C, 
 0b10001,
 0b01110,
 0b01111,
 0b01111,
 0b01111,
 0b01110,
 0b10001,
 0b00000
);

CGRAM_PATTERN(HD44780_INVERTED_V, 
 0b01110,
 0b01110,
 0b01110,
 0b01110,
 0b01110,
 0b10101,
 0b11011,
 0b00000
);


CGRAM_PATTERN(HD44780_INVERTED_O,
 0b10001,
 0b01110,
 0b01110,
 0b01110,
 0b01110,
 0b01110,
 0b10001,
 0b00000
);


CGRAM_PATTERN(HD44780_INVERTED_N,
 0b01110,
 0b01110,
 0b00110,
 0b01010,
 0b01100,
 0b01110,
 0b01110,
 0b00000
);
