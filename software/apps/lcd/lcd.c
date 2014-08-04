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

#include "core/clock.h"
#include "core/process.h"
#include "core/spi_slave.h"
#include "core/timer.h"
#include "drivers/hd44780.h"
#include "hal/interrupt.h"
#include "hal/gpio.h"

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

#define DEBUG0 C,5

PROCESS(spi_handler);

static hd44780_lcd lcd;


static void print_string(char* s)
{
  while (*s) {
    hd44780_lcd_write(&lcd, *s);
    s += 1;
  }
}

static void print_lcd_welcome(void)
{

  hd44780_lcd_set_ddram_address(&lcd, 0x04);
  print_string("Hello world!");
}


PROCESS_THREAD(spi_handler)
{
  PROCESS_BEGIN();

  static uint8_t response_error_counter = 0;
  static char buf[4];

  while (true) {
    PROCESS_WAIT_EVENT();

    if (ev == SPIS_MESSAGE_RECEIVED) {
      spis_send_response(0x00, NULL, 0);
      hd44780_lcd_set_ddram_address(&lcd, 0x00);
      print_string("Msg received: ");
      if (spis_get_rx_size() == 1) {
        hd44780_lcd_write(&lcd, *spis_get_rx_buf());
	hd44780_lcd_write(&lcd, ' ');
	hd44780_lcd_write(&lcd, ' ');
      } else {
	hd44780_lcd_write(&lcd, 'E');
	hd44780_lcd_write(&lcd, 'R');
	hd44780_lcd_write(&lcd, 'R');
      }
    } else if (ev == SPIS_RESPONSE_TRANSMITTED) {
      hd44780_lcd_set_ddram_address(&lcd, 0x40);
      print_string("Response transmitted");
    } else if (ev == SPIS_RESPONSE_ERROR) {
      if (response_error_counter < UINT8_MAX) {
	response_error_counter += 1;
      }
      hd44780_lcd_set_ddram_address(&lcd, 0x14);
      sprintf(buf,"%d",response_error_counter);
      print_string("Response errors: ");
      print_string(buf);
    } else {
      hd44780_lcd_set_ddram_address(&lcd, 0x40);
      print_string("Unknown event       ");
    }
  }

  PROCESS_END();
}


static void init_lcd(void)
{
  hd44780_init();
  hd44780_lcd_setup(&lcd, &LCD_DATA_PORT, &LCD_CTRL_PORT, LCD_FIRST_DATA_PIN,
		    LCD_E_PIN, LCD_RS_PIN, LCD_RW_PIN);
  hd44780_lcd_init(&lcd, HD44780_TWO_ROWS);
  hd44780_lcd_set_entry_mode(&lcd, HD44780_RIGHT, NO_SHIFT_DISPLAY);
  hd44780_lcd_set_display(&lcd, ENABLE_DISPLAY, DISABLE_CURSOR,
			  DISABLE_CURSOR_BLINK);
}


// For debugging:
extern volatile uint8_t* spis_trx_status;

static void init_pins(void)
{
  SET_PIN_DIR_OUTPUT(DEBUG0);
  CLR_PIN(DEBUG0);
}

int main(void)
{
  ENABLE_INTERRUPTS();
  init_pins();
  clock_init();
  process_init();
  process_start(&spi_handler);
  spis_init(&spi_handler);


  // Init LCD
  init_lcd();
  print_lcd_welcome();
  
  // Event loop
  while (true) {
    process_execute();
  } 
}

