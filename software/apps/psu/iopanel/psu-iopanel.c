/*
 * psu-iopanel.c
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
 * @file psu-iopanel.c
 * @author Pieter Agten (pieter.agten@gmail.com)
 * @date 30 Jun 2014
 *
 * This is the firmware for the PSU's IO-panel.
 */

#include <stdlib.h>

#include "apps/psu/packets.h"
#include "hal/gpio.h" 
#include "hal/fuses.h"
#include "hal/interrupt.h"
#include "core/clock.h"
#include "core/io_monitor.h"
#include "core/knob.h"
#include "core/rotary.h"
#include "core/spi_slave.h"
#include "core/timer.h"
#include "drivers/hd44780.h"
#include "util/log.h"

// NOTE: the default fuse values defined in avr-libc are incorrect (see the 
// ATmega328p datasheet)
FUSES = 
{
  .extended = 0xFF, // BOD disabled
  .high = FUSE_SPIEN, // SPIEN enabled
  .low = FUSE_CKSEL0, // Full swing crystal oscillator, slowly rising power
};


// ****************** Pins ******************
#define LCD_DATA_PORT       PORTD
#define LCD_CTRL_PORT       PORTD
#define LCD_FIRST_DATA_PIN  0
#define LCD_E_PIN           4
#define LCD_RS_PIN          7
#define LCD_RW_PIN          6

#define ROTV_A    C,0
#define ROTV_B    C,1
#define ROTV_PUSH C,2
// ******************************************

// ******* Voltage and current limits *******
#define VOLTAGE_MIN             0
#define VOLTAGE_MAX         15000
#define VOLTAGE_SMALL_STEP    100 //10
#define VOLTAGE_BIG_STEP      100

#define CURRENT_MIN            0
#define CURRENT_MAX         3000
#define CURRENT_SMALL_STEP     5
#define CURRENT_BIG_STEP      50
// ******************************************



PROCESS(inputs_process);
PROCESS(spi_handler);
PROCESS(lcd_process);

static hd44780_lcd lcd;

static knob knob_v;
static knob knob_c;

static iomon_event rot_tick;

#define EVENT_ROTARY_TICK 0x00

static struct {
  uint8_t flags;
  uint16_t set_voltage;
  uint16_t set_current;
  uint16_t voltage;
  uint16_t current;
} psu_status;

static uint8_t trx_success = 0;
static uint8_t trx_failed = 0;


static inline
void init_pins(void)
{
  SET_PIN_DIR_INPUT(ROTV_A);
  SET_PIN_DIR_INPUT(ROTV_B);
  SET_PIN_DIR_INPUT(ROTV_PUSH);

  //  SET_PIN_DIR_INPUT(ROTC_A);
  //  SET_PIN_DIR_INPUT(ROTC_B);
  //  SET_PIN_DIR_INPUT(ROTC_PUSH);
}

static inline
void init_lcd(void)
{
  hd44780_init();
  hd44780_lcd_setup(&lcd, &LCD_DATA_PORT, &LCD_CTRL_PORT, LCD_FIRST_DATA_PIN,
		    LCD_E_PIN, LCD_RS_PIN, LCD_RW_PIN);
  hd44780_lcd_init(&lcd, HD44780_TWO_ROWS);
  hd44780_lcd_set_entry_mode(&lcd, HD44780_RIGHT, NO_SHIFT_DISPLAY);
  hd44780_lcd_set_display(&lcd, ENABLE_DISPLAY, DISABLE_CURSOR,
			  DISABLE_CURSOR_BLINK);
}


PROCESS_THREAD(inputs_process)
{
  PROCESS_BEGIN();
  static knob* k = &knob_v;

  while (true) {
    PROCESS_WAIT_EVENT_UNTIL(ev == EVENT_ROTARY_TICK);

    if (DEBOUNCED(data) & _BV(GET_BIT(ROTV_PUSH)) &&
	TOGGLED(data) & _BV(GET_BIT(ROTV_PUSH))) {
      // ROTV push button pressed
      if (k == &knob_v) {
	k = &knob_c;
      } else {
	k = &knob_v;
      }
    }

    uint8_t input_v = rot_input(DEBOUNCED(data), GET_BIT(ROTV_A),
				GET_BIT(ROTV_B));
    knob_update(k, input_v);

    //uint8_t input_c = rot_input(DEBOUNCED(data), GET_BIT(ROTC_A), GET_BIT(ROTC_B));
    //knob_update(&knob_c, input_c);
  }

  PROCESS_END();
}


PROCESS_THREAD(spi_handler)
{
  PROCESS_BEGIN();

  static struct iopanel_response response;

  while (true) {
    PROCESS_WAIT_EVENT_UNTIL(ev == SPIS_MESSAGE_RECEIVED);

    if (spis_get_rx_type() == IOPANEL_REQUEST_TYPE &&
	spis_get_rx_size() == sizeof(struct iopanel_request)) {
      struct iopanel_request* pkt = (struct iopanel_request*)spis_get_rx_buf();
      psu_status.flags = pkt->flags;
      psu_status.set_voltage = pkt->set_voltage;
      psu_status.set_current = pkt->set_current;
      psu_status.voltage = pkt->voltage;
      psu_status.current = pkt->current;
    }

    response.set_flags = 0;
    response.set_voltage = knob_get_value(&knob_v);
    response.set_current = knob_get_value(&knob_c);
    spis_send_response(IOPANEL_RESPONSE_TYPE, (uint8_t*)&response,
		       sizeof(struct iopanel_response));


    PROCESS_WAIT_EVENT_UNTIL(ev == SPIS_RESPONSE_TRANSMITTED || 
			     ev == SPIS_RESPONSE_ERROR);
    if (ev == SPIS_RESPONSE_TRANSMITTED) {
      trx_success += 1;
    } else {
      trx_failed += 1;
    }
  }

  PROCESS_END();
}


static inline
void format_value(char buf[8], int16_t v)
{
  sprintf(buf, "% 6.4d", v);
  buf[7] = 0;
  buf[6] = buf[5];
  buf[5] = buf[4];
  buf[4] = buf[3];
  buf[3] = '.';
}

PROCESS_THREAD(lcd_process)
{
  PROCESS_BEGIN();
  char buf0[8];
  char buf1[8];
  
  while (true) {
    // Voltage
    PROCESS_YIELD();
    hd44780_lcd_set_ddram_address(&lcd, HD44780_20X4_LINE0);
    format_value(buf0, psu_status.voltage);
    format_value(buf1, psu_status.set_voltage);
    fprintf(hd44780_lcd_stream(&lcd), "V: %s (%s)", buf0, buf1);

    // Current
    PROCESS_YIELD();
    hd44780_lcd_set_ddram_address(&lcd, HD44780_20X4_LINE1);
    format_value(buf0, psu_status.current);
    format_value(buf1, psu_status.set_current);
    fprintf(hd44780_lcd_stream(&lcd), "C: %s (%s)", buf0, buf1);

    // Transfer stats
    PROCESS_YIELD();
    hd44780_lcd_set_ddram_address(&lcd, HD44780_20X4_LINE2);
    fprintf(hd44780_lcd_stream(&lcd), "Trx: %3u s %3u f",
	    trx_success, trx_failed);
    

    // Transfer errors
    /*    static char str[21];
    static uint8_t i;
    PROCESS_WAIT_UNTIL(! hd44780_lcd_busy(&lcd));
    hd44780_lcd_set_ddram_address(&lcd, HD44780_20X4_LINE0);
    sprintf(str, "%u / %u / %u ",
	    trx_success,
	    log_cntr_get_value(LOG_CNTR_SPIS_TIMEOUT_WAITING_FOR_CALLBACK),
	    log_cntr_get_value(LOG_CNTR_EVENT_QUEUE_FULL));

    PROCESS_YIELD();
    i = 0;
    while(str[i] != 0) {
      PROCESS_WAIT_UNTIL(! hd44780_lcd_busy(&lcd));
      hd44780_lcd_write(&lcd, str[i]);
      i += 1;
      }*/
  }

  PROCESS_END();
}


int main(void)
{
  ENABLE_INTERRUPTS();
  init_pins();
  clock_init();
  process_init();
  knob_init(&knob_v, VOLTAGE_MIN, VOLTAGE_MAX, VOLTAGE_SMALL_STEP,
	    VOLTAGE_BIG_STEP);
  knob_init(&knob_c, CURRENT_MIN, CURRENT_MAX, CURRENT_SMALL_STEP,
	    CURRENT_BIG_STEP);
  iomon_init();
  init_lcd();

  process_start(&inputs_process);
  process_start(&spi_handler);
  process_start(&lcd_process);
  spis_init(&spi_handler);

  iomon_event_init(&rot_tick, PORT_PTR_TO_IOMON_PORT(&GET_PORT(ROTV_A)),
		   GET_PIN_MASK(GET_BIT(ROTV_A), GET_BIT(ROTV_B),
				GET_BIT(ROTV_PUSH)),
		   &inputs_process, EVENT_ROTARY_TICK);
  iomon_event_enable(&rot_tick);

  while (true) {
    process_execute();
  }
}
