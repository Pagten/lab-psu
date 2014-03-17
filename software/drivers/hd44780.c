/*
 * hd44780.c
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
 * @file hd44780.c
 * @author Pieter Agten (pieter.agten@gmail.com)
 * @date 13 mar 2014
 *
 */

#include "hd44780.h"

#include <avr/cpufunc.h>
#include <util/delay.h>
#include "util/bit.h"


void hd44780_init(void)
{ }


hd44780_setup_status
hd44780_lcd_setup(hd44780_lcd* lcd, port_ptr hnibble_port, port_ptr ctrl_port,
		  uint8_t hnibble_pin, uint8_t e_pin, uint8_t rs_pin,
		  uint8_t rw_pin)
{
  if (hnibble_pin > 4)
    return HD44780_SETUP_HNIBBLE_PIN_INVALID;
  if (e_pin > 7)
    return HD44780_SETUP_E_PIN_INVALID;
  if (rs_pin > 7)
    return HD44780_SETUP_RS_PIN_INVALID;
  if (rw_pin > 7)
    return HD44780_SETUP_RW_PIN_INVALID;

  lcd->data_port = hnibble_port;
  lcd->ctrl_port = ctrl_port;
  lcd->data_shift = hnibble_pin;
  lcd->data_mask = (0x0F << hnibble_pin);
  lcd->e_mask = bv8(e_pin);
  lcd->rs_mask = bv8(rs_pin);
  lcd->rw_mask = bv8(rw_pin);
  return HD44780_SETUP_OK;
}

/**
 * Send half of an 8-bit instruction: only the upper nibble will be sent.
 */
static
void write_half(hd44780_lcd* lcd, uint8_t val)
{
  // The implementation is slow enough not to require any NOPs
  P_CLR_PINS(lcd->ctrl_port, lcd->rw_mask);
  P_SET_PINS_DIR_OUTPUT(lcd->data_port, lcd->data_mask);

  P_SET_PINS(lcd->ctrl_port, lcd->e_mask);  // E = 1
  uint8_t shifted_val = (val & 0xF0) >> (4 - lcd->data_shift);
  uint8_t p_val = (P_GET_VAL(lcd->data_port) & ~(lcd->data_mask)) | shifted_val;
  P_SET_VAL(lcd->data_port, p_val);         // Set data
  P_CLR_PINS(lcd->ctrl_port, lcd->e_mask);  // E = 0
}

static
void write(hd44780_lcd* lcd, uint8_t val)
{
  // The implementation is slow enough not to require any NOPs
  P_CLR_PINS(lcd->ctrl_port, lcd->rw_mask);
  P_SET_PINS_DIR_OUTPUT(lcd->data_port, lcd->data_mask);

  P_SET_PINS(lcd->ctrl_port, lcd->e_mask);  // E = 1
  uint8_t shifted_val_h = (val & 0xF0) >> (4 - lcd->data_shift);
  uint8_t p_val_h = (P_GET_VAL(lcd->data_port) & ~(lcd->data_mask)) | shifted_val_h;
  P_SET_VAL(lcd->data_port, p_val_h);       // Set upper nibble
  P_CLR_PINS(lcd->ctrl_port, lcd->e_mask);  // E = 0


  P_SET_PINS(lcd->ctrl_port, lcd->e_mask);  // E = 1
  uint8_t shifted_val_l = (val & 0x0F) << lcd->data_shift;
  uint8_t p_val_l = (P_GET_VAL(lcd->data_port) & ~(lcd->data_mask)) | shifted_val_l;
  P_SET_VAL(lcd->data_port, p_val_l);       // Set lower nibble
  P_CLR_PINS(lcd->ctrl_port, lcd->e_mask);  // E = 0
}

static
uint8_t read(hd44780_lcd* lcd)
{
  // The implementation is slow enough to only require nops after setting E = 1
  // We need to wait 360ns + 2 instructions between setting E = 1 and reading
  // the data lines, normal instructions already take at least 200ns, so
  // there's at most 160ns + 2 instructions left to wait.
  uint8_t bf_addr;
  P_SET_PINS_DIR_INPUT(lcd->data_port, lcd->data_mask);
  P_SET_PINS(lcd->ctrl_port, lcd->rw_mask);

  P_SET_PINS(lcd->ctrl_port, lcd->e_mask);  // E = 1
  _delay_us(0.160); _NOP(); _NOP();
  bf_addr = (P_GET_VAL(lcd->data_port) << (4 - lcd->data_shift)) & 0xF0;
  P_CLR_PINS(lcd->ctrl_port, lcd->e_mask);  // E = 0


  P_SET_PINS(lcd->ctrl_port, lcd->e_mask);  // E = 1
  _delay_us(0.160); _NOP(); _NOP();
  bf_addr |= (P_GET_VAL(lcd->data_port) >> lcd->data_shift) & 0x0F;
  P_CLR_PINS(lcd->ctrl_port, lcd->e_mask);  // E = 0

  return bf_addr;
}


void hd44780_lcd_init(hd44780_lcd* lcd, hd44780_nb_rows nb_rows)
{
  P_SET_PINS_DIR_OUTPUT(lcd->ctrl_port, lcd->e_mask|lcd->rs_mask|lcd->rw_mask);
  P_CLR_PINS(lcd->ctrl_port, lcd->rs_mask | lcd->rw_mask);

  // See the HD44780 datasheet for the initialization procedure
  _delay_ms(40.0);
  write_half(lcd, 0x30);
  _delay_ms(4.1);
  write_half(lcd, 0x30);
  _delay_us(100.0);
  write_half(lcd, 0x30);

  _delay_us(37.0);
  write_half(lcd, 0x20);
  //We have now switched to 4-bit mode

  _delay_us(37.0);
  write(lcd, 0x20 | nb_rows);
  _delay_us(37.0);
  write(lcd, 0x08);
  _delay_us(37.0);
  write(lcd, 0x01);
  _delay_ms(1.52);
  write(lcd, 0x06);

  // Initialization done, busy flag can now be checked instead of waiting
  // between commands
}


void hd44780_lcd_clear(hd44780_lcd* lcd)
{
  while (hd44780_lcd_busy(lcd));
  write(lcd, 0x01);
}


void hd44780_lcd_home(hd44780_lcd* lcd)
{
  while (hd44780_lcd_busy(lcd));
  write(lcd, 0x02);
}


void hd44780_lcd_set_entry_mode(hd44780_lcd* lcd, hd44780_direction cursor_dir,
				bool shift_display)
{
  while (hd44780_lcd_busy(lcd));
  write(lcd, 0x04 | cursor_dir | shift_display);
}


void hd44780_lcd_set_display(hd44780_lcd* lcd, bool display, bool cursor,
			     bool cursor_blink)
{
  while (hd44780_lcd_busy(lcd));
  write(lcd, 0x08 | (display << 2) | (cursor << 1) | cursor_blink);
}


void hd44780_lcd_move_cursor(hd44780_lcd* lcd, hd44780_direction dir)
{
  while (hd44780_lcd_busy(lcd));
  write(lcd, 0x18 | (dir << 1));
}


void hd44780_lcd_shift_display(hd44780_lcd* lcd, hd44780_direction dir)
{
  while (hd44780_lcd_busy(lcd));
  write(lcd, 0x10 | (dir << 1));
}


void hd44780_lcd_set_ddram_address(hd44780_lcd* lcd, uint8_t address)
{
  while (hd44780_lcd_busy(lcd));
  write(lcd, 0x80 | address);
}


void hd44780_lcd_set_cgram_address(hd44780_lcd* lcd, uint8_t address)
{
  while (hd44780_lcd_busy(lcd));
  write(lcd, 0x40 | (address & 0x3F));
}


void hd44780_lcd_write(hd44780_lcd* lcd, uint8_t data)
{
  while (hd44780_lcd_busy(lcd));
  P_SET_PINS(lcd->ctrl_port, lcd->rs_mask);
  write(lcd, data);
}


// Calling this function makes the RS signal low
bool hd44780_lcd_busy(hd44780_lcd* lcd)
{
  P_CLR_PINS(lcd->ctrl_port, lcd->rs_mask);
  return !!(read(lcd) & 0x80);
}


uint8_t hd44780_lcd_read_address(hd44780_lcd* lcd)
{
  while (hd44780_lcd_busy(lcd));
  _delay_us(4.0); // Wait 4us for the address to be updated
  return read(lcd) & 0x07F;
}


uint8_t hd44780_lcd_read(hd44780_lcd* lcd)
{
  while (hd44780_lcd_busy(lcd));
  P_SET_PINS(lcd->ctrl_port, lcd->rs_mask);
  return read(lcd);
}
