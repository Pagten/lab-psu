/*
 * hd44780.h
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

#ifndef HD44780_H
#define HD44780_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "hal/gpio.h"
#include "util/ring_buffer.h"

/**
 * @file hd44780.h
 * @author Pieter Agten (pieter.agten@gmail.com)
 * @date 24 Jul 2015
 *
 * Driver for Hitachi HD44780 compatible dot matrix LCD controllers. The driver
 * operates the LCD in read/write 4-bit mode and assumes 5x8 dot characters.
 *
 * The lower four data lines of the LCD are not used and hence should be tied
 * to ground through a current-limiting resistor. The upper four data lines
 * must be connected to four consecutive pins of the same IO port on the MCU.
 * The E, RS and RW lines must be connected to pins on the same IO port on the
 * MCU (but the control port can be different from the data port). 
 */

#define HD44780_20X4_LINE0 0x00
#define HD44780_20X4_LINE1 0x40
#define HD44780_20X4_LINE2 0x14
#define HD44780_20X4_LINE3 0x54

typedef struct hd44780_lcd {
  port_ptr data_port;
  port_ptr ctrl_port;
  uint8_t data_shift;
  uint8_t data_mask;
  uint8_t e_mask;
  uint8_t rs_mask;
  uint8_t rw_mask;
  bool tx_pending;
  ring_buffer instr_buf;
  FILE stream;  
} hd44780_lcd;

typedef enum {
  HD44780_ONE_ROW  = 0,
  HD44780_TWO_ROWS = 8,
} hd44780_nb_rows;

typedef enum {
  HD44780_LEFT  = 0,
  HD44780_RIGHT = 2,
} hd44780_direction;

typedef enum {
  HD44780_SETUP_OK,
  HD44780_SETUP_HNIBBLE_PIN_INVALID,
  HD44780_SETUP_E_PIN_INVALID,
  HD44780_SETUP_RS_PIN_INVALID,
  HD44780_SETUP_RW_PIN_INVALID,
  HD44780_SETUP_INSTR_BUF_TOO_SMALL,
} hd44780_setup_status;


/**
 * Initialize the HD44780 driver module.
 *
 * Modules that must be initialized first:
 *  * process
 */
void hd44780_init(void);


/*#define HD44780_SETUP(HNIBBLE_PORT, CTRL_PORT, HNIBBLE_PIN, E_PIN, RS_PIN, RW_PIN, INSTR_BUF_SIZE) \
  { .data_port = HNIBBLE_PORT, \
    .ctrl_port = CTRL_PORT, \
    .data_shift = HNIBBLE_PIN, \
    .data_mask = (0x0F << HNIBBLE_PIN),	\
    .e_mask = _BV(E_PIN),		\
    .rs_mask = _BV(RS_PIN),		\
    .rw_mask = _BV(RW_PIN),		\
    .tx_pending = false,		\
    .instr_buf = RINGBUF_INIT(INSTR_BUF_SIZE) \
    }*/

/**
 * Set up a HD44780 LCD driver structure.
 *
 * This function only initializes a HD44780 LCD data structure, it does not 
 * configure the actual LCD device. To configure the LCD device, use the 
 * hd44780_lcd_init() function.
 *
 * @param data_port    Address of the IO port connected to the four upper data
 *                     lines of the LCD.
 * @param ctrl_port    Address of the control port, connected to the E, RS and
 *                     RW lines of the LCD.
 * @param hnibble_pin  Pin of the hnibble port connected to D4. The next three
 *                     pins are assumed to be connected to the next three data
 *                     lines.
 * @param e_pin        Control port pin connected to the E line.
 * @param rs_pin       Control port pin connected to the RS line.
 * @param rw_pin       Control port pin connected to the RW line.
 * @param instr_buf    Pointer to the buffer for the instruction queue.
 * @param instr_buf_sz Size of the instruction queue.
 * @return HD44780_SETUP_OK if the initializion was successful, or a different
 *         value indicating an error otherwise.
 */
hd44780_setup_status
hd44780_lcd_setup(hd44780_lcd* lcd, port_ptr data_port, port_ptr ctrl_port,
		  uint8_t hnibble_pin, uint8_t e_pin, uint8_t rs_pin,
		  uint8_t rw_pin, uint8_t* instr_buf, size_t instr_buf_sz);


/**
 * Initialize a HD44780 LCD.
 *
 * The initialization includes setting the correct pin directions. Any HD44780
 * LCD must be initialized before any of the other lcd functions can be used
 * on it.
 * 
 * @param lcd      The LCD device to initialize.
 * @param nb_rows  The number of rows on the LCD device. Note that a four row
 *                 device is actually implemented as a two row device with 
 *                 double-length row (the first row continues on the third row
 *                 and the second row continues on the fourth row).
 */
void hd44780_lcd_init(hd44780_lcd* lcd, hd44780_nb_rows nb_rows);


///**
// * Disable a given HD44780 LCD, so that it won't further consume CPU cycles. It
// * can be re-enabled by calling hd44780_lcd_init().
// *
// * @param lcd The LCD device to disable.
// */
//void hd44780_lcd_disable(hd44780_lcd* lcd);

/**
 * Clear the display of an HD44780 LCD.
 *
 * @param lcd  The LCD device of which to clear the display.
 */
void hd44780_lcd_clear(hd44780_lcd* lcd);


/**
 * Move the cursor of an HD44780 LCD to the home position.
 *
 * @param lcd  The LCD device of which to move the cursor to the home position.
 */
void hd44780_lcd_home(hd44780_lcd* lcd);


/**
 * Set the character entry mode of an HD44780 LCD.
 *
 * @param lcd            The LCD device of which to set the character
 *                       entry mode.
 * @param cursor_dir     Determines the direction in which the cursor moves
 *                       after writing a character. If shift_display is set
 *                       to true, the entire display shifts in the other
 *                       direction such that the cursor stays at the same
 *                       location.
 * @param shift_display  If this parameter is set true, the entire display
 *                       contents will shift when a character is written, such
 *                       that the cursor stays at the same location.
 */
void hd44780_lcd_set_entry_mode(hd44780_lcd* lcd, hd44780_direction cursor_dir,
				bool shift_display);
#define SHIFT_DISPLAY true
#define NO_SHIFT_DISPLAY false



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
#define ENABLE_DISPLAY true
#define DISABLE_DISPLAY false
#define ENABLE_CURSOR true
#define DISABLE_CURSOR false
#define ENABLE_CURSOR_BLINK true
#define DISABLE_CURSOR_BLINK false


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
 * Return whether an HD44780 LCD is currently busy. Note that, unlike most
 * other functions of this module, this function is synchronous (i.e., it
 * immediately communicates with the LCD device to check if it is busy).
 *
 * @param lcd  The LCD device of which to query the busy flag.
 * @return true if the LCD is busy, false otherwise.
 */
bool hd44780_lcd_busy(hd44780_lcd* lcd);

/**
 * Return a file stream that can be used to write to the DDRAM or CGRAM of an
 * HD44780 LCD.
 *
 * The returned file stream is actually just a wrapper around the 
 * hd44780_lcd_write() function, hence the documentation of that function also
 * applies here. In particular, the functions  hd44780_lcd_set_ddram_address()
 * and hd44780_lcd_set_cgram_address() determine what kind of memory and what
 * location in that memory is written when writing to the stream. Depending
 * on the entry mode set using the hd44780_set_entry_mode(), the LCD screen
 * will behave differently when writing to the stream.
 */
FILE* hd44780_lcd_stream(hd44780_lcd* lcd);

#endif
