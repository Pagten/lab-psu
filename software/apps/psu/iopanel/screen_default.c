/*
 * screen_default.c
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
 * @file screen_default.c
 * @author Pieter Agten (pieter.agten@gmail.com)
 * @date 05 Aug 2015
 *
 */

#include "screen_default.h"

#include "core/process.h"
#include "drivers/hd44780.h"

#define LINE1 HD44780_20X4_LINE0
#define LINE2 HD44780_20X4_LINE1
#define LINE3 HD44780_20X4_LINE2
#define LINE4 HD44780_20X4_LINE3

#define ADDR_VOLTAGE_TITLE     (LINE1 + 3)
#define ADDR_CURRENT_TITLE     (LINE1 + 13)
#define ADDR_SET_VOLTAGE       (LINE2 + 3)
#define ADDR_SET_CURRENT       (LINE2 + 14)
#define ADDR_MEASURED_VOLTAGE  (LINE3 + 3)
#define ADDR_MEASURED_CURRENT  (LINE3 + 14)
#define ADDR_MEASURED_POWER    (LINE4 + 7)

static hd44780_lcd* lcd;

/**
 * Print a decimal fixed point value with 3 fractional digits. The output
 * will always be 7 characters long (+ a string delimiter) and will be
 * right aligned.
 */
static void sprint_fixp(char buf[8], int16_t voltage)
{
  sprintf(buf, "% 6.4d", v);
  buf[7] = 0;
  buf[6] = buf[5];
  buf[5] = buf[4];
  buf[4] = buf[3];
  buf[3] = '.';
}

static void draw_voltage(void)
{
  FILE* flcd = hd44780_lcd_stream(lcd);
  char strbuf[8];

  // Line 1: title
  hd44780_lcd_set_ddram_address(lcd, ADDR_VOLTAGE_TITLE);
  fputs(flcd, "Voltage");

  // Line 2: set value
  sprint_fixp(strbuf, psu_get_set_voltage());
  hd44780_lcd_set_ddram_address(lcd, ADDR_SET_VOLTAGE);
  fprintf(flcd, "%sV", strbuf);
}

static void draw_current(void)
{
  FILE* flcd = hd44780_lcd_stream(lcd);

  // Line 1: Title
  hd44780_lcd_set_ddram_address(lcd, ADDR_CURRENT_TITLE);
  fputs(flcd, "Current");

  // Line 2: Set value
  sprint_fixp(strbuf, psu_get_set_current());
  hd44780_lcd_set_ddram_address(lcd, ADDR_SET_CURRENT);
  fprintf(flcd, "%sA", strbuf);
}

static void draw(void)
{
  FILE* flcd = hd44780_lcd_stream(lcd);

  // Line 1
  // Voltage title
  hd44780_lcd_set_ddram_address(lcd, ADDR_VOLTAGE_TITLE);
  fputs(flcd, "Voltage");

  // Current title
  hd44780_lcd_set_ddram_address(lcd, ADDR_CURRENT_TITLE);
  fputs(flcd, "Current");

  // Line 2
  // Set voltage
  hd44780_lcd_set_ddram_address(lcd, ADDR_SET_VOLTAGE);
  int16_t set_voltage = psu_get_set_voltage();
  fprintf(flcd, "%7.4dV", );
}

PROCESS_THREAD(screen_default_process)
{
  PROCESS_BEGIN();

  while(true) {
    PROCESS_WAIT_EVENT();
    switch(ev) {
    case EVENT_SCREEN_LEFT_ROTARY_TICK:
      {
	rot_step_status direction = (rot_step_status)data;
      }
      break;
    case EVENT_SCREEN_RIGHT_ROTARY_TICK:
      {
	rot_step_status direction = (rot_step_status)data;
      }
      break;
    case EVENT_SCREEN_LEFT_ROTARY_CLICK:
      break;
    case EVENT_SCREEN_RIGHT_ROTARY_CLICK:
      break;
    case EVENT_SCREEN_PSU_STATUS_UPDATED:
      break;
    case EVENT_SCREEN_REDRAW:
      break;
    }
  }

  PROCESS_END();
}
