/*
 * spi_master_test.c
 *
 * Copyright 2013 Pieter Agten
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
 * @file spi_master_test.c
 * @author Pieter Agten <pieter.agten@gmail.com>
 * @date 3 nov 2013
 *
 * Unit test for the spi master module.
 */

#include <stdint.h>
#include <check.h>

#include <stdio.h> // for testing, temporary

#include "core/process.h"
#include "core/spi_master.h"
#include "hal/mock_timer.h"
#include "hal/spi.h"
#include "util/math.h"

#include "spi_master_test.h"

#define SPI_MOCK_TX_DATA_BUFFER_SIZE 32
#define SPI_DUMMY_PIN 2
#define DUMMY_DATA_SIZE 16
#define DUMMY_DELAY 5
#define PROC_CALL_MARGIN 4
#define NO_DELAY 0

static uint8_t dummy_port;
static uint8_t dummy_data[DUMMY_DATA_SIZE] = { 
  0x42, 0x84, 0x21, 0x11, 0x44, 0x88, 0xBB, 0xFF,
  0x12, 0x34, 0x45, 0x56, 0x67, 0x78, 0x89, 0x9A
};

static void setup(void)
{
  spi_mock_init(SPI_MOCK_TX_DATA_BUFFER_SIZE);
  spi_mock_set_incoming_data(dummy_data, 16);
  clock_init();
  process_init();
  spim_init();
  dummy_port = 0xFF;
}

static void teardown(void)
{ }

// ****************************************************************************
//                       test_trx_init_invalid
// ****************************************************************************
START_TEST(test_trx_init_invalid)
{
  spim_trx trx;
  spim_trx_init_status init_stat;
  init_stat = spim_trx_init(&trx, SPI_DUMMY_PIN, &dummy_port, 
			    NULL, 0,   // transmit
			    NULL, 0,   // receive
			    NO_DELAY);
  ck_assert(init_stat == SPIM_TRX_INIT_INVALID);
  ck_assert(spim_trx_get_status(&trx) == SPIM_TRX_STATUS_INVALID);
}
END_TEST

// ****************************************************************************
//                       test_trx_queue_invalid_status
// ****************************************************************************
START_TEST(test_trx_queue_invalid_status)
{
  spim_trx trx;

  trx.status = SPIM_TRX_STATUS_INVALID;
  ck_assert(spim_trx_queue(&trx) == SPIM_TRX_QUEUE_INVALID_STATUS);

  trx.status = SPIM_TRX_STATUS_QUEUED;
  ck_assert(spim_trx_queue(&trx) == SPIM_TRX_QUEUE_INVALID_STATUS);

  trx.status = SPIM_TRX_STATUS_IN_TRANSMISSION;
  ck_assert(spim_trx_queue(&trx) == SPIM_TRX_QUEUE_INVALID_STATUS);

  trx.status = SPIM_TRX_STATUS_COMPLETED;
  ck_assert(spim_trx_queue(&trx) == SPIM_TRX_QUEUE_INVALID_STATUS);
}
END_TEST


// ****************************************************************************
//                       test_send_single_byte
// ****************************************************************************
START_TEST(test_send_single_byte)
{
  spim_trx trx;
  spim_trx_init_status init_stat;
  init_stat = spim_trx_init(&trx, SPI_DUMMY_PIN, &dummy_port, 
			    dummy_data, 1, // transmit
			    NULL, 0,       // receive
			    NO_DELAY);
  ck_assert(init_stat == SPIM_TRX_INIT_OK);
  ck_assert(spim_trx_get_status(&trx) == SPIM_TRX_STATUS_INITIAL);

  spim_trx_queue_status queue_stat = spim_trx_queue(&trx);
  ck_assert(queue_stat == SPIM_TRX_QUEUE_OK);
  ck_assert(spim_trx_get_status(&trx) == SPIM_TRX_STATUS_QUEUED);

  int i = 0;
  do {
    if (i > PROC_CALL_MARGIN) ck_abort_msg("Transmission timeout");
    PROCESS_CALL(&spim_trx_process);
    i += 1;
  } while (spim_trx_get_status(&trx) == SPIM_TRX_STATUS_IN_TRANSMISSION);
   
  ck_assert(spim_trx_get_status(&trx) == SPIM_TRX_STATUS_COMPLETED);
  ck_assert(spi_mock_get_last_transmitted_data(0) == dummy_data[0]);
}
END_TEST


// ****************************************************************************
//                       test_receive_single_byte
// ****************************************************************************
START_TEST(test_receive_single_byte)
{
  spim_trx trx;
  spim_trx_init_status init_stat;
  uint8_t rx_buf;
  init_stat = spim_trx_init(&trx, SPI_DUMMY_PIN, &dummy_port, 
			    NULL, 0,    // transmit
			    &rx_buf, 1, // receive
			    NO_DELAY);
  ck_assert(init_stat == SPIM_TRX_INIT_OK);
  ck_assert(spim_trx_get_status(&trx) == SPIM_TRX_STATUS_INITIAL);

  spim_trx_queue_status queue_stat = spim_trx_queue(&trx);
  ck_assert(queue_stat == SPIM_TRX_QUEUE_OK);
  ck_assert(spim_trx_get_status(&trx) == SPIM_TRX_STATUS_QUEUED);

  int i = 0;
  do {
    if (i > PROC_CALL_MARGIN) ck_abort_msg("Transmission timeout");
    PROCESS_CALL(&spim_trx_process);
    i += 1;
  } while (spim_trx_get_status(&trx) == SPIM_TRX_STATUS_IN_TRANSMISSION);
   
  ck_assert(spim_trx_get_status(&trx) == SPIM_TRX_STATUS_COMPLETED);
  ck_assert(rx_buf == dummy_data[0]);
}
END_TEST


// ****************************************************************************
//                       test_send_bytes
// ****************************************************************************
START_TEST(test_send_bytes)
{
  spim_trx trx;
  spim_trx_init_status init_stat;
  init_stat = spim_trx_init(&trx, SPI_DUMMY_PIN, &dummy_port, 
			    dummy_data, DUMMY_DATA_SIZE, // transmit
			    NULL, 0,                     // receive
			    NO_DELAY);
  ck_assert(init_stat == SPIM_TRX_INIT_OK);
  ck_assert(spim_trx_get_status(&trx) == SPIM_TRX_STATUS_INITIAL);

  spim_trx_queue_status queue_stat = spim_trx_queue(&trx);
  ck_assert(queue_stat == SPIM_TRX_QUEUE_OK);
  ck_assert(spim_trx_get_status(&trx) == SPIM_TRX_STATUS_QUEUED);

  int i = 0;
  do {
    if (i > DUMMY_DATA_SIZE + PROC_CALL_MARGIN)
      ck_abort_msg("Transmission timeout");
    PROCESS_CALL(&spim_trx_process);
    i += 1;
  } while (spim_trx_get_status(&trx) == SPIM_TRX_STATUS_IN_TRANSMISSION);
   
  ck_assert(spim_trx_get_status(&trx) == SPIM_TRX_STATUS_COMPLETED);
  for (int j = 0; j < DUMMY_DATA_SIZE; ++j) {
    ck_assert(spi_mock_get_last_transmitted_data(DUMMY_DATA_SIZE-1 - j) == dummy_data[j]);
  }
}
END_TEST


// ****************************************************************************
//                       test_receive_bytes
// ****************************************************************************
START_TEST(test_receive_bytes)
{
  spim_trx trx;
  spim_trx_init_status init_stat;
  uint8_t rx_buf[DUMMY_DATA_SIZE];
  init_stat = spim_trx_init(&trx, SPI_DUMMY_PIN, &dummy_port, 
			    NULL, 0,                 // transmit
			    rx_buf, DUMMY_DATA_SIZE, // receive
			    NO_DELAY);
  ck_assert(init_stat == SPIM_TRX_INIT_OK);
  ck_assert(spim_trx_get_status(&trx) == SPIM_TRX_STATUS_INITIAL);

  spim_trx_queue_status queue_stat = spim_trx_queue(&trx);
  ck_assert(queue_stat == SPIM_TRX_QUEUE_OK);
  ck_assert(spim_trx_get_status(&trx) == SPIM_TRX_STATUS_QUEUED);

  int i = 0;
  do {
    if (i > DUMMY_DATA_SIZE + PROC_CALL_MARGIN)
      ck_abort_msg("Transmission timeout");
    PROCESS_CALL(&spim_trx_process);
    i += 1;
  } while (spim_trx_get_status(&trx) == SPIM_TRX_STATUS_IN_TRANSMISSION);
   
  ck_assert(spim_trx_get_status(&trx) == SPIM_TRX_STATUS_COMPLETED);
  for (int j = 0; j < DUMMY_DATA_SIZE; ++j) {
    ck_assert(rx_buf[j] == dummy_data[j]);
  }
}
END_TEST



// ****************************************************************************
//                       test_send_receive
// ****************************************************************************
START_TEST(test_send_receive)
{
  spim_trx trx;
  spim_trx_init_status init_stat;
  uint8_t rx_buf[DUMMY_DATA_SIZE];
  init_stat = spim_trx_init(&trx, SPI_DUMMY_PIN, &dummy_port, 
			    dummy_data, DUMMY_DATA_SIZE/2, // transmit
			    rx_buf, DUMMY_DATA_SIZE,       // receive
			    NO_DELAY);
  ck_assert(init_stat == SPIM_TRX_INIT_OK);
  ck_assert(spim_trx_get_status(&trx) == SPIM_TRX_STATUS_INITIAL);

  spim_trx_queue_status queue_stat = spim_trx_queue(&trx);
  ck_assert(queue_stat == SPIM_TRX_QUEUE_OK);
  ck_assert(spim_trx_get_status(&trx) == SPIM_TRX_STATUS_QUEUED);

  int i = 0;
  do {
    if (i > DUMMY_DATA_SIZE + PROC_CALL_MARGIN)
      ck_abort_msg("Transmission timeout");
    PROCESS_CALL(&spim_trx_process);
    i += 1;
  } while (spim_trx_get_status(&trx) == SPIM_TRX_STATUS_IN_TRANSMISSION);
   
  ck_assert(spim_trx_get_status(&trx) == SPIM_TRX_STATUS_COMPLETED);
  for (int j = 0; j < DUMMY_DATA_SIZE; ++j) {
    ck_assert(rx_buf[j] == dummy_data[j]);
    ck_assert(spi_mock_get_last_transmitted_data(DUMMY_DATA_SIZE - j) == (j < DUMMY_DATA_SIZE/2 ? dummy_data[j] : 0));
  }
  ck_assert(spi_mock_get_last_transmitted_data(0) == 0);
}
END_TEST



// ****************************************************************************
//                       test_trx_multiple
// ****************************************************************************
START_TEST(test_trx_multiple)
{
  spim_trx trx0, trx1;
  spim_trx_init_status init_stat;
  spim_trx_queue_status queue_stat;
  uint8_t rx_buf0[DUMMY_DATA_SIZE], rx_buf1[DUMMY_DATA_SIZE];

  // init trx0
  init_stat = spim_trx_init(&trx0, SPI_DUMMY_PIN, &dummy_port, 
			    dummy_data, DUMMY_DATA_SIZE, // transmit
			    rx_buf0, DUMMY_DATA_SIZE,    // receive
			    NO_DELAY);
  ck_assert(init_stat == SPIM_TRX_INIT_OK);
  ck_assert(spim_trx_get_status(&trx0) == SPIM_TRX_STATUS_INITIAL);

  // init trx1
  init_stat = spim_trx_init(&trx1, SPI_DUMMY_PIN, &dummy_port, 
			    dummy_data, DUMMY_DATA_SIZE, // transmit
			    rx_buf1, DUMMY_DATA_SIZE,    // receive
			    NO_DELAY);
  ck_assert(init_stat == SPIM_TRX_INIT_OK);
  ck_assert(spim_trx_get_status(&trx1) == SPIM_TRX_STATUS_INITIAL);

  // queue trx0
  queue_stat = spim_trx_queue(&trx0);
  ck_assert(queue_stat == SPIM_TRX_QUEUE_OK);
  ck_assert(spim_trx_get_status(&trx0) == SPIM_TRX_STATUS_QUEUED);

  // queue trx1
  queue_stat = spim_trx_queue(&trx1);
  ck_assert(queue_stat == SPIM_TRX_QUEUE_OK);
  ck_assert(spim_trx_get_status(&trx1) == SPIM_TRX_STATUS_QUEUED);

  // transmit trx0
  int i = 0;
  do {
    if (i > DUMMY_DATA_SIZE + PROC_CALL_MARGIN)
      ck_abort_msg("Transmission timeout");
    PROCESS_CALL(&spim_trx_process);
    i += 1;
  } while (spim_trx_get_status(&trx0) == SPIM_TRX_STATUS_IN_TRANSMISSION);
  
  ck_assert(spim_trx_get_status(&trx0) == SPIM_TRX_STATUS_COMPLETED);
  for (int j = 0; j < DUMMY_DATA_SIZE; ++j) {
    ck_assert(rx_buf0[j] == dummy_data[j]);
    ck_assert(spi_mock_get_last_transmitted_data(DUMMY_DATA_SIZE - j) == dummy_data[j]);
  }
  ck_assert(spi_mock_get_last_transmitted_data(0) == 0);


  // transmit trx1
  spi_mock_set_incoming_data(dummy_data, 16);
  ck_assert(spim_trx_get_status(&trx1) == SPIM_TRX_STATUS_QUEUED);
  i = 0;
  do {
    if (i > DUMMY_DATA_SIZE + PROC_CALL_MARGIN)
      ck_abort_msg("Transmission timeout");
    PROCESS_CALL(&spim_trx_process);
    i += 1;
  } while (spim_trx_get_status(&trx1) == SPIM_TRX_STATUS_IN_TRANSMISSION);
  
  ck_assert(spim_trx_get_status(&trx1) == SPIM_TRX_STATUS_COMPLETED);
  for (int j = 0; j < DUMMY_DATA_SIZE; ++j) {
    ck_assert(rx_buf1[j] == dummy_data[j]);
    ck_assert(spi_mock_get_last_transmitted_data(DUMMY_DATA_SIZE - j) == dummy_data[j]);
  }
  ck_assert(spi_mock_get_last_transmitted_data(0) == 0);
}
END_TEST


// ****************************************************************************
//                       test_trx_delay
// ****************************************************************************
START_TEST(test_trx_delay)
{
  spim_trx trx;
  spim_trx_init_status init_stat;
  uint8_t rx_buf[DUMMY_DATA_SIZE];
  init_stat = spim_trx_init(&trx, SPI_DUMMY_PIN, &dummy_port, 
			    dummy_data, DUMMY_DATA_SIZE/2, // transmit
			    rx_buf, DUMMY_DATA_SIZE,       // receive
			    DUMMY_DELAY);
  ck_assert(init_stat == SPIM_TRX_INIT_OK);
  ck_assert(spim_trx_get_status(&trx) == SPIM_TRX_STATUS_INITIAL);

  spim_trx_queue_status queue_stat = spim_trx_queue(&trx);
  ck_assert(queue_stat == SPIM_TRX_QUEUE_OK);
  ck_assert(spim_trx_get_status(&trx) == SPIM_TRX_STATUS_QUEUED);

  int i = 0;
  do {
    for (int j = 0; j < DUMMY_DELAY*2; ++j) {
      PROCESS_CALL(&spim_trx_process);
    }
    ck_assert(spi_mock_get_nb_bytes_transmitted() == MIN(i, DUMMY_DATA_SIZE+1));
    for (int j = 0; j < DUMMY_DELAY*3; ++j) {
      MOCK_TIMER_TICK(CLOCK_TMR);
    }
    i += 1;
  } while (spim_trx_get_status(&trx) == SPIM_TRX_STATUS_IN_TRANSMISSION);
   
  ck_assert(spim_trx_get_status(&trx) == SPIM_TRX_STATUS_COMPLETED);
  for (int j = 0; j < DUMMY_DATA_SIZE; ++j) {
    ck_assert(rx_buf[j] == dummy_data[j]);
    ck_assert(spi_mock_get_last_transmitted_data(DUMMY_DATA_SIZE - j) == (j < DUMMY_DATA_SIZE/2 ? dummy_data[j] : 0));
  }
  ck_assert(spi_mock_get_last_transmitted_data(0) == 0);
}
END_TEST



// ****************************************************************************
//                           Test suite setup
// ****************************************************************************
static void add_tcase(Suite* s, TFun tf, const char* name)
{
  TCase *tc = tcase_create(name);
  tcase_add_checked_fixture(tc, setup, teardown);
  tcase_add_test(tc, tf);
  suite_add_tcase(s, tc);
}

Suite *spi_master_suite(void)
{
  Suite *s = suite_create("Spi master");

  add_tcase(s, test_trx_init_invalid,         "Trx init invalid");
  add_tcase(s, test_trx_queue_invalid_status, "Trx queue invalid status");
  add_tcase(s, test_send_single_byte,         "Send single byte");
  add_tcase(s, test_receive_single_byte,      "Receive single byte");
  add_tcase(s, test_send_bytes,               "Send bytes");
  add_tcase(s, test_receive_bytes,            "Receive bytes");
  add_tcase(s, test_send_receive,             "Send/receive bytes");
  add_tcase(s, test_trx_multiple,             "Send/receive multiple trx");
  add_tcase(s, test_trx_delay,                "Send/receive with delay");

  return s;
}
