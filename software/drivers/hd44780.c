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


void hd44780_init(void)
{ }


hd44780_setup_status
hd44780_lcd_setup(hd44780_lcd* lcd, port_ptr hnibble_port, uint8_t hnibble_pin,
		  port_ptr ctrl_port, uint8_t e_pin, uint8_t rs_pin,
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

  lcd->hnibble_port = hnibble_port;
  lcd->hnibble_shift = hnibble_pin;
  lcd->ctrl_port = ctrl_port;
  lcd->e_mask = bv8(e_pin);
  lcd->rs_mask = bv8(rs_pin);
  lcd->rw_mask = bv8(rw_pin);
  return HD44780_SETUP_OK;
}


static inline
void set_ctrl(hd44780_lcd* lcd, bool rs, bool rw)
{
  
}

void hd44780_lcd_init(hd44780_lcd* lcd, hd44780_nb_rows nb_rows)
{
  //E cycle times are halved if LCD voltage is close to 5V
  //1 instruction lasts about 62.5ns at 16Mhz

  P_SET_PINS_DIR_OUTPUT(lcd->ctrl_port, lcd->e_mask | lcd->rs_mask | lcd->rw_mask);
  P_CLR_PINS(lcd->ctrl_port, lcd->e_mask | lcd->rs_mask | lcd->rw_mask);

  //wait 40ms
  _delay_ms(40.0);

  //E = 1
  //set rs and rw to 0
  //set data to 0x3
  //E must remain high for at least 450ns (8 instructions at 16Mhz)
  //Data must be stable for at least 80ns before E can go low
  //E = 0

  //wait 4.1ms
 
  //E = 1
  //all pins still have the correct value
  //E must remain high for at least 450ns
  //Data must be stable for at least 80ns before E can go low
  //E = 0
  
  //wait 0.1ms 

  //E = 1
  //all pins still have the correct value
  //E must remain high for at least 450ns
  //Data must be stable for at least 80ns before E can go low
  //E = 0


  //There must be at least 1us between making E high (1 E clock cycle)
  //E = 1
  //set data to 0x2
  //E must remain high for at least 450ns
  //Data must be stable for at least 80ns before E can go low
  //E = 0

  //We have now switched to 4-bit mode: first send high nibble, then low nibble

  //high nibble:
  //There must be at least 1us between making E high (1 E clock cycle)
  //E = 1
  //all pins still have the correct value
  //E must remain high for at least 450ns
  //Data must be stable for at least 80ns before E can go low
  //E = 0
  //
  //low nibble:
  //There must be at least 1us between making E high (1 E clock cycle)
  //E = 1
  //data = nb_rows << 3
  //E must remain high for at least 450ns
  //Data must be stable for at least 80ns before E can go low
  //E = 0
  
  //Must wait at least 37us before processing next command!

  //high nibble:
  //E = 1
  //data = 0
  //E must remain high for at least 450ns
  //Data must be stable for at least 80ns before E can go low
  //E = 0
  //
  //low nibble:
  //There must be at least 1us between making E high (1 E clock cycle)
  //E = 1
  //data = 0x8
  //E must remain high for at least 450ns
  //Data must be stable for at least 80ns before E can go low
  //E = 0

  //Must wait at least 37us before processing next command!

  //high nibble:
  //E = 1
  //data = 0
  //E must remain high for at least 450ns
  //Data must be stable for at least 80ns before E can go low
  //E = 0
  //
  //low nibble:
  //There must be at least 1us between making E high (1 E clock cycle)
  //E = 1
  //data = 0x1
  //E must remain high for at least 450ns
  //Data must be stable for at least 80ns before E can go low
  //E = 0

  //Must wait at least 1.52ms before processing next command!!

  //high nibble:
  //E = 1
  //data = 0
  //E must remain high for at least 450ns
  //Data must be stable for at least 80ns before E can go low
  //E = 0
  //
  //low nibble:
  //There must be at least 1us between making E high (1 E clock cycle)
  //E = 1
  //data = 0x4 | ID << 1 | S
  //E must remain high for at least 450ns
  //Data must be stable for at least 80ns before E can go low
  //E = 0

  // Initialization done, busy flag can now be checked instead of waiting
  // between commands
}


/**
 * Clear the display of an HD44780 LCD.
 *
 * @param lcd  The LCD device of which to clear the display.
 */
void hd44780_lcd_clear(hd44780_lcd* lcd);

/**
 * Set the character entry mode of an HD44780 LCD.
 *
 * @param lcd            The LCD device of which to set the character
 *                       entry mode.
 * @param shift_display  If this parameter is set true, the entire display
 *                       contents will shift when a character is written, such
 *                       that the cursor stays at the same location.
 * @param cursor_dir     Determines the direction in which the cursor moves
 *                       after writing a character. If shift_display is set
 *                       to true, the entire display shifts in the other
 *                       direction such that the cursor stays at the same
 *                       location.
 */
void hd44780_lcd_set_entry_mode(hd44780_lcd* lcd, bool shift_display,
				hd44780_direction cursor_dir);


/**
 * Configure the display options of an HD44780 LCD.
 *
 * @param lcd           The LCD device of which to set the display options.
 * @param display       Enables or disables the entire display.
 * @param cursor        Enables or disables the cursor.
 * @param cursor_blink  Enables or disables cursor blinking.
 */
void hd44780_lcd_set_display(hd44780_lcd* lcd, bool display, bool cursor,
			     bool cursor_blink);


/**
 * Move the cursor of an HD44780 LCD one position to the left or right.
 *
 * @param lcd  The LCD device of which to move the cursor.
 * @param dir  The direction in which to move the cursor.
 */
void hd44780_lcd_move_cursor(hd44780_lcd* lcd, hd44780_direction dir);


/**
 * Shift the display of an HD44780 LCD one position to the left or right.
 *
 * @param lcd  The LCD device of which to shift the display.
 * @param dir  The direction in which to shift the display.
 */
void hd44780_lcd_shift_display(hd44780_lcd* lcd, hd44780_direction dir);


/**
 * Set the Data Display RAM (DDRAM) address of an HD44780 LCD.
 *
 * The DDRAM address determines the position on the display that will be
 * read/written by a subsequent call to the hd44780_lcd_read() and
 * hd44780_lcd_write() functions.
 *
 * @param lcd     The LCD device of which to set the DDRAM address.
 * @param address The DDRAM address to set. The first display line starts at
 *                address 0x00 and the second line starts at address 0x40. The
 *                most significant address bit is ignored.
 */
void hd44780_lcd_set_ddram_address(hd44780_lcd* lcd, uint8_t address);


/**
 * Set the Character Generator RAM (CGRAM) address of an HD44780 LCD.
 *
 * The character generator allows the HD44780 to be programmed with up to eight
 * 5x8 user defined characters. The CGRAM address determines which dots of
 * which custom character will be read/written by a subsequent call to the
 * hd44780_lcd_read() or hd44780_lcd_write() functions.
 * 
 * @param lcd     The LCD device of which to set the CGRAM address.
 * @param address The CGRAM address to set. The first 5x8 custom character
 *                starts at address 0x00 and each subsequent custom character
 *                starts 8 bytes later (i.e. 0x08, 0x10, 0x18, ...). The two
 *                most significant address bits are ignored. 
 */
void hd44780_lcd_set_cgram_address(hd44780_lcd* lcd, uint8_t address);


/**
 * Write to the DDRAM or CGRAM of an HD44780 LCD.
 *
 * The DDRAM or CGRAM address that will be written is the one selected
 * previously using the hd44780_lcd_set_ddram_address() or
 * hd44780_lcd_set_cgram_address() functions. If no address was selected using
 * these functions, the effect of calling this function is undefined.
 *
 * If DDRAM is written, the cursor might move or the display might shift,
 * depending the entry mode set using the hd44780_set_entry_mode() function.
 * The character set used can be found in the HD44780 datasheet, but is
 * typically a subset of ASCII. Character codes 0 to 7 and 8 to 15 are mapped
 * to the custom characters programmed in CGRAM.
 * 
 * If CGRAM is written, the CGRAM address is incremented or decremented
 * depending on the cursor move direction set using hd44780_set_entry_mode().
 * The display doesn't shift when writing CGRAM. The five least significant
 * data bits are mapped to the five dots making up the row addressed in CGRAM.
 * The value of the other three bits is ignored. 
 *
 * @param lcd   The LCD device of which to write the DDRAM or CGRAM.
 * @param data  The data to write. 
 */
void hd44780_lcd_write(hd44780_lcd* lcd, uint8_t data);


/**
 * Return whether an HD44780 LCD is currently busy.
 *
 * No commands may be sent to the LCD device when it is busy.
 *
 * @param lcd  The LCD device of which to query the busy flag.
 * @return true if the LCD is busy, false otherwise.
 */
bool hd44780_lcd_busy(hd44780_lcd* lcd);


/**
 * Read the current address counter of an HD44780 LCD.
 *
 * Whether the CGRAM or DDRAM address counter is returned depends on whether
 * the last executed set_address function was hd44780_lcd_set_ddram_address()
 * or hd44780_lcd_set_cgram_address().
 *
 * @param lcd  The LCD device of which to read the address counter.
 * @return The current value of the address counter. The MSB is always zero.
 */
uint8_t hd44780_lcd_read_address(hd44780_lcd* lcd);


/**
 * Read a value from the DDRAM or CGRAM of an HD44780 LCD.
 *
 * The DDRAM or CGRAM address that will be read is the one selected previously
 * using the hd44780_lcd_set_ddram_address() or hd44780_lcd_set_cgram_address()
 * functions. If the previous command was not one of these two functions, the
 * read data will be invalid, unless the previous command was an lcd_read
 * command itself.
 *
 * This command will increment or decrement the address counter according to
 * the entry mode configured using the hd44780_lcd_set_entry_mode() function.
 *
 * @param lcd  The LCD device of which to read data.
 * @return The data read from the DDRAM or CGRAM.
 */
uint8_t hd44780_lcd_read(hd44780_lcd* lcd)
