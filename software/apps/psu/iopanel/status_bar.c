/*
 * screen_default.c
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
 * @file status_bar.c
 * @author Pieter Agten (pieter.agten@gmail.com)
 * @date 07 Aug 2015
 */

#define ADDR_CV  HD44780_LINE1
#define ADDR_CC  HD44780_LINE2
#define ADDR_ON  HD44780_LINE3

void status_bar_draw(hd44780_lcd* lcd)
{
  FILE* flcd = hd44780_lcd_stream(lcd);

  // CV
  hd44780_lcd_set_ddram_address(lcd, ADDR_CV);
  fputs(flcd, "CV");

  // CC
  hd44780_lcd_set_ddram_address(lcd, ADDR_CC);
  fputs(flcd, "CC");

  // ON
  hd44780_lcd_set_ddram_address(lcd, ADDR_ON);
  fputs(flcd, "ON");
}
