/*
 * lcd.c
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
 * @file lcd.c
 * @author Pieter Agten (pieter.agten@gmail.com)
 * @date 15 Mar 2014
 *
 * This is the main file for a small LCD test program.
 */

#include "hal/interrupt.h"
#include "hal/gpio.h"
#include "core/clock.h"
#include "core/timer.h"
#include "drivers/hd44780.h"

#include <stdio.h>

#include <avr/io.h> // For fuses
#include <util/delay.h>

// NOTE: the default fuse values defined in avr-libc are incorrect (see the 
// ATmega328p datasheet)
FUSES = 
{
  .extended = 0xFF, // BOD disabled
  .high = FUSE_SPIEN, // SPIEN enabled
  .low = FUSE_CKSEL0, // Full swing crystal oscillator, slowly rising power
};


#define LCD_DATA_PORT       PORTD
#define LCD_CTRL_PORT       PORTD
#define LCD_FIRST_DATA_PIN  0
#define LCD_E_PIN           4
#define LCD_RS_PIN          7
#define LCD_RW_PIN          6


static hd44780_lcd lcd;



static void print_string(char* s)
{
  while (*s) {
    hd44780_lcd_write(&lcd, *s);
    s += 1;
  }
}

static void print_time(void)
{
  clock_time_t now = clock_get_time();
  char str[12];
  sprintf(str, "%lu", now);
  print_string(str);
}


// Wait 2 seconds using a timer
static void wait(void)
{
  static timer tmr;

  timer_set(&tmr, CLK_AT_LEAST(2.0 * CLOCK_SEC));
  while (! timer_expired(&tmr));

  //  _delay_ms(2000.0);
}

static void print_lcd_welcome(void)
{

  hd44780_lcd_set_ddram_address(&lcd, 0x04);
  hd44780_lcd_write(&lcd, 'H');
  hd44780_lcd_write(&lcd, 'e');
  hd44780_lcd_write(&lcd, 'l');
  hd44780_lcd_write(&lcd, 'l');
  hd44780_lcd_write(&lcd, 'o');
  hd44780_lcd_write(&lcd, ' ');
  hd44780_lcd_write(&lcd, 'w');
  hd44780_lcd_write(&lcd, 'o');
  hd44780_lcd_write(&lcd, 'r');
  hd44780_lcd_write(&lcd, 'l');
  hd44780_lcd_write(&lcd, 'd');
  hd44780_lcd_write(&lcd, '!');

  wait();
  hd44780_lcd_set_ddram_address(&lcd, 0x40);
  print_time();
  //  hd44780_lcd_write(&lcd, 'L');
  //  hd44780_lcd_write(&lcd, 'i');
  //  hd44780_lcd_write(&lcd, 'n');
  //  hd44780_lcd_write(&lcd, 'e');
  //  hd44780_lcd_write(&lcd, ' ');
  //  hd44780_lcd_write(&lcd, '2');

  wait();
  hd44780_lcd_set_ddram_address(&lcd, 0x14);
  print_time();
  //  hd44780_lcd_write(&lcd, 'L');
  //  hd44780_lcd_write(&lcd, 'i');
  //  hd44780_lcd_write(&lcd, 'n');
  //  hd44780_lcd_write(&lcd, 'e');
  //  hd44780_lcd_write(&lcd, ' ');
  //  hd44780_lcd_write(&lcd, '3');

  //  wait();
  //  hd44780_lcd_set_ddram_address(&lcd, 0x54);
  //  print_time();
  //hd44780_lcd_write(&lcd, 'L');
  //hd44780_lcd_write(&lcd, 'i');
  //hd44780_lcd_write(&lcd, 'n');
  //hd44780_lcd_write(&lcd, 'e');
  //hd44780_lcd_write(&lcd, ' ');
  //hd44780_lcd_write(&lcd, '4');
}

int main(void)
{
  ENABLE_INTERRUPTS();
  clock_init();


  // Init LCD
  hd44780_init();
  hd44780_lcd_setup(&lcd, &LCD_DATA_PORT, &LCD_CTRL_PORT, LCD_FIRST_DATA_PIN,
		    LCD_E_PIN, LCD_RS_PIN, LCD_RW_PIN);
  hd44780_lcd_init(&lcd, HD44780_TWO_ROWS);
  hd44780_lcd_set_entry_mode(&lcd, HD44780_RIGHT, NO_SHIFT_DISPLAY);
  hd44780_lcd_set_display(&lcd, ENABLE_DISPLAY, DISABLE_CURSOR,
			  DISABLE_CURSOR_BLINK);
 

  print_lcd_welcome();

  while (true) {
    _delay_ms(2000.0);
    hd44780_lcd_set_ddram_address(&lcd, 0x54);
    print_string("        ");
    hd44780_lcd_set_ddram_address(&lcd, 0x54);
    print_time();
  } 

  while (true);
}

