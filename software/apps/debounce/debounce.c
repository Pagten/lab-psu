/*
 * debounce.c
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
 * @file debounce.c
 * @author Pieter Agten (pieter.agten@gmail.com)
 * @date 05 Apr 2014
 *
 * This is the main file for a small test program that increments a number of
 * counters based on the input from a number of software-debounced pins and
 * writes the counter value to an LCD.
 */

#include "hal/gpio.h"
#include "drivers/hd44780.h"

// NOTE: the default fuse values defined in avr-libc are incorrect (see the 
// ATmega328p datasheet)
FUSES = 
{
  .extended = 0xFF, // BOD disabled
  .high = FUSE_SPIEN, // SPIEN enabled
  .low = FUSE_CKSEL0, // Full swing crystal oscillator, slowly rising power
};


#define LCD_DATA_PORT       PORTD
#define LCD_CTRL_PORT       PORTC
#define LCD_FIRST_DATA_PIN  1
#define LCD_E_PIN           1
#define LCD_RS_PIN          5
#define LCD_RW_PIN          4

#define ROT0A C,3
#define ROT0B C,2
#define BTN0  C,6


static hd44780_lcd lcd;


static inline
void init_pins(void)
{
  SET_PIN_DIR_INPUT(ROT0A);
  SET_PIN_DIR_INPUT(ROT0B);
  SET_PIN_DIR_INPUT(BTN0);
}

static inline
void init_lcd(void)
{
  hd44780_init();

  hd44780_lcd_setup(&lcd, &LCD_DATA_PORT, &LCD_CTRL_PORT, LCD_FIRST_DATA_PIN,
		    LCD_E_PIN, LCD_RS_PIN, LCD_RW_PIN);
  hd44780_lcd_init(&lcd, HD44780_TWO_ROWS);
  hd44780_lcd_set_entry_mode(&lcd, HD44780_RIGHT, false);
  hd44780_lcd_set_display(&lcd, true, false, false);
  hd44780_lcd_clear(&lcd);
}

int main(void)
{
  init_pins();
  init_lcd();


  while (true);
}

