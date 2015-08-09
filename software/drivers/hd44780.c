/*
 * hd44780_async.c
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
 * @file hd44780_async.c
 * @author Pieter Agten (pieter.agten@gmail.com)
 * @date 24 Jul 2015
 */

#include "hd44780.h"

//#include <stdio.h>
//#include <stddef.h>
#include <util/delay.h>
#include "core/process.h"
#include "hal/cpufunc.h"
#include "util/bit.h"
#include "util/def.h"
#include "util/fdev_stream.h"
#include "util/ring_buffer.h"

enum instr_type {
  INSTR_TYPE_COMMAND,
  INSTR_TYPE_DATA,
};

#define EVENT_INSTR_SUBMITTED 0x00

/**
 * Below we define sentinel values to indicate special instructions in the
 * instruction queue. These sentinel values can never occur as normal data in
 * the instruction queue, because they are non-printable ASCII control
 * characters.
 */
#define SENTINEL_CMD              0x11 /** Next val is command, not data */
#define SENTINEL_ENTER_CGRAM_MODE 0x12 /** Save DDRAM address */
#define SENTINEL_EXIT_CGRAM_MODE  0x20 /** Restore DDRAM address */



#define HD44780_SET_DDRAM_BIT       7
#define HD44780_SET_CGRAM_BIT       6
#define HD44780_DIRECTION_RIGHT_BIT 1
#define HD44780_BUSY_FLAG           7
#define HD44780_ADDR_MASK           (~_BV(HD44780_BUSY_FLAG))

PROCESS(hd44780_process);

static int hd44780_putchar(char c, FILE* stream);

void hd44780_init(void)
{
  process_start(&hd44780_process);
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

hd44780_setup_status
hd44780_lcd_setup(hd44780_lcd* lcd, port_ptr data_port, port_ptr ctrl_port,
		  uint8_t hnibble_pin, uint8_t e_pin, uint8_t rs_pin,
		  uint8_t rw_pin, uint8_t* instr_buf, size_t instr_buf_sz,
		  hd44780_cgram* cgram)
{
  if (hnibble_pin > 4)
    return HD44780_SETUP_HNIBBLE_PIN_INVALID;
  if (e_pin > 7)
    return HD44780_SETUP_E_PIN_INVALID;
  if (rs_pin > 7)
    return HD44780_SETUP_RS_PIN_INVALID;
  if (rw_pin > 7)
    return HD44780_SETUP_RW_PIN_INVALID;
  if (instr_buf_sz < 2) 
    return HD44780_SETUP_INSTR_BUF_TOO_SMALL;

  lcd->data_port = data_port;
  lcd->ctrl_port = ctrl_port;
  lcd->data_shift = hnibble_pin;
  lcd->data_mask = bv8(hnibble_pin);
  lcd->e_mask = bv8(e_pin);
  lcd->rs_mask = bv8(rs_pin);
  lcd->rw_mask = bv8(rw_pin);
  lcd->has_stored_addr = false;
  lcd->current_addr_type = HD44780_DDRAM;
  lcd->tx_pending = false;

  ringbuf_init(&(lcd->instr_buf), instr_buf, instr_buf_sz);
  fdev_setup_stream(&(lcd->stream), hd44780_putchar, NULL, _FDEV_SETUP_WRITE);
  lcd->cgram = cgram;
  return HD44780_SETUP_OK;
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

  lcd->current_addr_type = HD44780_DDRAM;
  lcd->direction = HD44780_RIGHT;
  // Initialization done, busy flag can now be checked instead of waiting
  // between commands
}

/**
 * Send the instruction at the tail of the queue to the LCD device. The queue
 * must not be empty when calling this function!
 */
static void
send_queued_instruction(hd44780_lcd* lcd, uint8_t readout)
{
  ring_buffer* const instr_buf = &(lcd->instr_buf);
  const uint8_t instr = ringbuf_get_byte(instr_buf);
  if (! lcd->in_cgram_mode && instr == SENTINEL_CMD) {
    // This sentinel value means the instruction is a command instead of data,
    // so we need to clear the RS line.
    P_CLR_PINS(lcd->ctrl_port, lcd->rs_mask);
    if (ringbuf_empty(instr_buf)) {
      // This should never happen, because it means a sentinel value was
      // placed in the instruction buffer, without a subsequent command!
      // TODO: log!
    }
    const uint8_t cmd = ringbuf_get_byte(instr_buf);
    if (bit_is_set(cmd, HD44780_SET_DDRAM_BIT)) {
      // Set DDRAM address command
      lcd->current_addr_type = HD44780_DDRAM;
    } else if (bit_is_set(cmd, HD44780_SET_CGRAM_BIT)) {
      // Set CGRAM address command
      lcd->current_addr_type = HD44780_CGRAM;
    }
    if ((cmd & ~0b11) = 0x04) {
      // Set entry mode command
      if (bit_is_set(cmd, HD44780_DIRECTION_RIGHT_BIT)) {
	lcd->direction = HD44780_RIGHT;
      } else {
	lcd->direction = HD44780_LEFT;
      }
    }

    // Write command to LCD
    write(lcd, cmd);
  } else if (! lcd->in_cgram_mode && instr == SENTINEL_ENTER_CGRAM_MODE) {
    // Save current address
    lcd->stored_addr_type = lcd->current_addr_type;
    lcd->stored_addr = readout & HD44780_ADDR_MASK;
    lcd->in_cgram_mode = true;
  } else if (lcd->in_cgram_mode && instr == SENTINEL_EXIT_CGRAM_MODE) {
    // Restore saved address
    uint8_t cmd;
    if (lcd->stored_addr_type == HD44780_DDRAM) {
      // DDRAM address
      cmd = _BV(HD44780_SET_DDRAM_BIT) | lcd->stored_addr;
    } else {
      // CGRAM address
      cmd = _BV(HD44780_SET_CGRAM_BIT) | (lcd->stored_addr & 
					  ~_BV(HD44780_SET_DDRAM_BIT));
    }
    lcd->in_cgram_mode = false;
    write(lcd, cmd);
  } else {
    // Any non-sentinel value means the instruction is data, so we need to set
    // the RS line.
    P_SET_PINS(lcd->ctrl_port, lcd->rs_mask);
    write(lcd, instr);
  }

  if (ringbuf_empty(&(lcd->instr_buf))) {
    lcd->tx_pending = false;
  }
}


/**
 * Push an instruction onto the queue of instructions to be sent to the LCD.
 */
static void
push_instruction(hd44780_lcd* lcd, enum instr_type type, uint8_t data)
{
  while (ringbuf_free_space(&(lcd->instr_buf)) < 2) {
    uint8_t readout;
    // Wait until LCD is no longer busy
    while ((readout = hd44780_lcd_read(lcd)) & HD44780_BUSY_FLAG);

    // Now synchronously execute an instruction from the instruction queue
    send_queued_instruction(lcd, readout);
  }

  if (type == INSTR_TYPE_COMMAND) {
    ringbuf_put_byte(&(lcd->instr_buf), SENTINEL_CMD);
    ringbuf_put_byte(&(lcd->instr_buf), data);
  } else {
    if (! lcd->in_cgram_mode && data == SENTINEL_CMD) {
      // Should not happen because the sentinel value is an unprintable ASCII
      // control character.
      // TODO: log!
      return;
    }
    ringbuf_put_byte(&(lcd->instr_buf), data);
  }

  if (! lcd->tx_pending) {
    process_post_event(&hd44780_process, EVENT_INSTR_SUBMITTED,
		       (process_data_t)lcd);
    lcd->tx_pending = true;
  }
}


void hd44780_lcd_clear(hd44780_lcd* lcd)
{
  push_instruction(lcd, INSTR_TYPE_COMMAND, 0x01); 
}


void hd44780_lcd_home(hd44780_lcd* lcd)
{
  push_instruction(lcd, INSTR_TYPE_COMMAND, 0x02); 
}


void hd44780_lcd_set_entry_mode(hd44780_lcd* lcd, hd44780_direction cursor_dir,
				bool shift_display)
{
  push_instruction(lcd, INSTR_TYPE_COMMAND, 0x04 | cursor_dir | shift_display); 
}


void hd44780_lcd_set_display(hd44780_lcd* lcd, bool display, bool cursor,
			     bool cursor_blink)
{
  push_instruction(lcd, INSTR_TYPE_COMMAND, 0x08 | (display << 2) | (cursor << 1) | cursor_blink);
}


void hd44780_lcd_move_cursor(hd44780_lcd* lcd, hd44780_direction dir)
{
  push_instruction(lcd, INSTR_TYPE_COMMAND, 0x18 | (dir << 1));
}


void hd44780_lcd_shift_display(hd44780_lcd* lcd, hd44780_direction dir)
{
  push_instruction(lcd, INSTR_TYPE_COMMAND, 0x10 | (dir << 1));
}


void hd44780_lcd_set_ddram_address(hd44780_lcd* lcd, uint8_t address)
{
  push_instruction(lcd, INSTR_TYPE_COMMAND, 0x80 | address);
}


void hd44780_cgram_write(hd44780_lcd* lcd, uint8_t index,
			 uint8_t pattern[8])
{
  if (index >= HD44780_NB_CGRAM_ENTRIES) {
    return;
  }

  // Save current address
  push_instruction(lcd, INSTR_TYPE_DATA, SENTINEL_ENTER_CGRAM_MODE); 

  if (lcd->direction == HD44780_RIGHT) {
    // Set CGRAM address
    const uint8_t addr = index << 3;
    push_instruction(lcd, INSTR_TYPE_COMMAND, 0x40 | addr);

    // Write each pattern line
    for (uint8_t i = 0; i < 8; ++i) {
      push_instruction(lcd, INSTR_TYPE_DATA, (pattern[i] & 0x1F)); 
    }
  } else {
    const uint8_t addr = (index << 3) + 8;
    push_instruction(lcd, INSTR_TYPE_COMMAND, 0x40 | addr);

    // Write each pattern line
    for (uint8_t i = 8; i > 0; --i) {
      push_instruction(lcd, INSTR_TYPE_DATA, (pattern[i] & 0x1F)); 
    }
  }

  // Restore saved address
  push_instruction(lcd, INSTR_TYPE_DATA, SENTINEL_EXIT_CGRAM_MODE); 
}

void hd44780_lcd_set_cgram_address(hd44780_lcd* lcd, uint8_t address)
{
  push_instruction(lcd, INSTR_TYPE_COMMAND, 0x40 | (address & 0x3F));
}


void hd44780_lcd_write(hd44780_lcd* lcd, uint8_t data)
{
  push_instruction(lcd, INSTR_TYPE_DATA, data);
}


inline uint8_t
hd44780_lcd_read(hd44780_lcd* lcd)
{
  P_CLR_PINS(lcd->ctrl_port, lcd->rs_mask);
  return read(lcd);
}

inline bool
hd44780_lcd_busy(uint8_t readout)
{
  return !!(readout & _BV(HD44780_BUSY_FLAG));
}


FILE* hd44780_lcd_stream(hd44780_lcd* lcd)
{
  return &(lcd->stream);
}

static int
hd44780_putchar(char c, FILE* stream)
{
  hd44780_lcd* lcd = container_of(stream, hd44780_lcd, stream);
  hd44780_lcd_write(lcd, (uint8_t)c);
  return 0;
}


PROCESS_THREAD(hd44780_process)
{
  PROCESS_BEGIN();

  while(true) {
    PROCESS_WAIT_EVENT();
    hd44780_lcd* const lcd = (hd44780_lcd*)data;

    uint8_t readout = hd44780_lcd_read(lcd);
    if (! hd44780_lcd_busy(readout)) {
      send_queued_instruction(lcd, readout);
    }

    if (lcd->tx_pending) {
      PROCESS_RESUBMIT_EVENT();
    }
  }

  PROCESS_END();
}
