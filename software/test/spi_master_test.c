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
 * Unit tests for the spi master module.
 */

#include <stdint.h>
#include <check.h>

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
//                       test_trx_set_invalid
// ****************************************************************************
START_TEST(test_trx_set_invalid)
{
  spim_trx_simple trx;
  spim_trx_simple_set_status set_stat;
  spim_trx_init((spim_trx*)&trx);
  set_stat = spim_trx_simple_set(&trx, SPI_DUMMY_PIN, &dummy_port, 
				 1, NULL,   // transmit
				 0, NULL,   // receive
				 NULL); // process
  ck_assert(set_stat == SPIM_TRX_SIMPLE_SET_TX_BUF_IS_NULL);


  set_stat = spim_trx_simple_set(&trx, SPI_DUMMY_PIN, &dummy_port, 
				 0, NULL,   // transmit
				 1, NULL,   // receive
				 NULL); // process
  ck_assert(set_stat == SPIM_TRX_SIMPLE_SET_RX_BUF_IS_NULL);
}
END_TEST

// ****************************************************************************
//                       test_trx_queue_already_queued
// ****************************************************************************
START_TEST(test_trx_queue_already_queued)
{
  spim_trx_simple trx;
  spim_trx_init((spim_trx*)&trx);
  ck_assert(spim_trx_queue((spim_trx*)&trx) == SPIM_TRX_QUEUE_OK);
  ck_assert(spim_trx_queue((spim_trx*)&trx) == SPIM_TRX_QUEUE_ALREADY_QUEUED);
}
END_TEST


// ****************************************************************************
//                       test_send_single_byte
// ****************************************************************************
START_TEST(test_send_single_byte)
{
  spim_trx_simple trx;
  spim_trx_simple_set_status set_stat;
  spim_trx_init((spim_trx*)&trx);
  set_stat = spim_trx_simple_set(&trx, SPI_DUMMY_PIN, &dummy_port, 
				 1, dummy_data, // transmit
				 0,  NULL,      // receive
				 NULL);         // process
  ck_assert(set_stat == SPIM_TRX_SIMPLE_SET_OK);
  ck_assert(! spim_trx_is_in_transmission((spim_trx*)&trx));

  spim_trx_queue_status queue_stat = spim_trx_queue((spim_trx*)&trx);
  ck_assert(queue_stat == SPIM_TRX_QUEUE_OK);
  ck_assert(! spim_trx_is_in_transmission((spim_trx*)&trx));

  int i = 0;
  do {
    if (i > PROC_CALL_MARGIN) ck_abort_msg("Transmission timeout");
    process_execute();
    i += 1;
  } while (spim_trx_is_in_transmission((spim_trx*)&trx));
  
  ck_assert(spi_mock_get_last_transmitted_data(0) == dummy_data[0]);
}
END_TEST


// ****************************************************************************
//                       test_receive_single_byte
// ****************************************************************************
START_TEST(test_receive_single_byte)
{
  spim_trx_simple trx;
  spim_trx_simple_set_status set_stat;
  uint8_t rx_buf;
  spim_trx_init((spim_trx*)&trx);
  set_stat = spim_trx_simple_set(&trx, SPI_DUMMY_PIN, &dummy_port, 
				 0, NULL,    // transmit
				 1, &rx_buf, // receive
				 NULL);      // process
  ck_assert(set_stat == SPIM_TRX_SIMPLE_SET_OK);
  ck_assert(! spim_trx_is_in_transmission((spim_trx*)&trx));

  spim_trx_queue_status queue_stat = spim_trx_queue((spim_trx*)&trx);
  ck_assert(queue_stat == SPIM_TRX_QUEUE_OK);
  ck_assert(! spim_trx_is_in_transmission((spim_trx*)&trx));

  int i = 0;
  do {
    if (i > PROC_CALL_MARGIN) ck_abort_msg("Transmission timeout");
    process_execute();
    i += 1;
  } while (spim_trx_is_in_transmission((spim_trx*)&trx));
  
  ck_assert(rx_buf == dummy_data[0]);
}
END_TEST


// ****************************************************************************
//                       test_send_bytes
// ****************************************************************************
START_TEST(test_send_bytes)
{
  spim_trx_simple trx;
  spim_trx_simple_set_status set_stat;
  spim_trx_init((spim_trx*)&trx);
  set_stat = spim_trx_simple_set(&trx, SPI_DUMMY_PIN, &dummy_port, 
				 DUMMY_DATA_SIZE, dummy_data, // transmit
				 0, NULL,                     // receive
				 NULL); // process
  ck_assert(set_stat == SPIM_TRX_SIMPLE_SET_OK);
  ck_assert(! spim_trx_is_in_transmission((spim_trx*)&trx));

  spim_trx_queue_status queue_stat = spim_trx_queue((spim_trx*)&trx);
  ck_assert(queue_stat == SPIM_TRX_QUEUE_OK);
  ck_assert(! spim_trx_is_in_transmission((spim_trx*)&trx));

  int i = 0;
  do {
    if (i > DUMMY_DATA_SIZE + PROC_CALL_MARGIN)
      ck_abort_msg("Transmission timeout");
    process_execute();
    i += 1;
  } while (spim_trx_is_in_transmission((spim_trx*)&trx));
   
  for (int j = 0; j < DUMMY_DATA_SIZE; ++j) {
    ck_assert(spi_mock_get_last_transmitted_data(DUMMY_DATA_SIZE-1 - j) == 
	      dummy_data[j]);
  }
}
END_TEST


// ****************************************************************************
//                       test_receive_bytes
// ****************************************************************************
START_TEST(test_receive_bytes)
{
  spim_trx_simple trx;
  spim_trx_simple_set_status set_stat;
  uint8_t rx_buf[DUMMY_DATA_SIZE];
  spim_trx_init((spim_trx*)&trx);
  set_stat = spim_trx_simple_set(&trx, SPI_DUMMY_PIN, &dummy_port, 
				 0, NULL,                  // transmit
				 DUMMY_DATA_SIZE, rx_buf,  // receive
				 NULL);
  ck_assert(set_stat == SPIM_TRX_SIMPLE_SET_OK);
  ck_assert(! spim_trx_is_in_transmission((spim_trx*)&trx));
  
  spim_trx_queue_status queue_stat = spim_trx_queue((spim_trx*)&trx);
  ck_assert(queue_stat == SPIM_TRX_QUEUE_OK);
  ck_assert(! spim_trx_is_in_transmission((spim_trx*)&trx));


  int i = 0;
  do {
    if (i > DUMMY_DATA_SIZE + PROC_CALL_MARGIN)
      ck_abort_msg("Transmission timeout");
    process_execute();
    i += 1;
  } while (spim_trx_is_in_transmission((spim_trx*)&trx));
   
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
  spim_trx_simple trx;
  spim_trx_simple_set_status set_stat;
  uint8_t rx_buf[DUMMY_DATA_SIZE];
  spim_trx_init((spim_trx*)&trx);
  set_stat = spim_trx_simple_set(&trx, SPI_DUMMY_PIN, &dummy_port, 
				 DUMMY_DATA_SIZE/2, dummy_data, // transmit
				 DUMMY_DATA_SIZE, rx_buf,       // receive
				 NULL); // process
  ck_assert(set_stat == SPIM_TRX_SIMPLE_SET_OK);
  ck_assert(! spim_trx_is_in_transmission((spim_trx*)&trx));

  spim_trx_queue_status queue_stat = spim_trx_queue((spim_trx*)&trx);
  ck_assert(queue_stat == SPIM_TRX_QUEUE_OK);
  ck_assert(! spim_trx_is_in_transmission((spim_trx*)&trx));

  int i = 0;
  do {
    if (i > DUMMY_DATA_SIZE + PROC_CALL_MARGIN)
      ck_abort_msg("Transmission timeout");
    process_execute();
    i += 1;
  } while (spim_trx_is_in_transmission((spim_trx*)&trx));
  
  for (int j = 0; j < DUMMY_DATA_SIZE; ++j) {
    ck_assert(rx_buf[j] == dummy_data[j]);
    ck_assert(spi_mock_get_last_transmitted_data(DUMMY_DATA_SIZE - j) == 
	      (j < DUMMY_DATA_SIZE/2 ? dummy_data[j] : 0));
  }
  ck_assert(spi_mock_get_last_transmitted_data(0) == 0);
}
END_TEST



// ****************************************************************************
//                       test_trx_multiple
// ****************************************************************************
START_TEST(test_trx_multiple)
{
  spim_trx_simple trx0, trx1;
  spim_trx_simple_set_status set_stat;
  spim_trx_queue_status queue_stat;
  uint8_t rx_buf0[DUMMY_DATA_SIZE], rx_buf1[DUMMY_DATA_SIZE];

  // set trx0
  spim_trx_init((spim_trx*)&trx0);
  set_stat = spim_trx_simple_set(&trx0, SPI_DUMMY_PIN, &dummy_port, 
				 DUMMY_DATA_SIZE, dummy_data, // transmit
				 DUMMY_DATA_SIZE, rx_buf0,    // receive
				 NULL); // process
  ck_assert(set_stat == SPIM_TRX_SIMPLE_SET_OK);
  ck_assert(! spim_trx_is_in_transmission((spim_trx*)&trx0));

  // set trx1
  spim_trx_init((spim_trx*)&trx1);
  set_stat = spim_trx_simple_set(&trx1, SPI_DUMMY_PIN, &dummy_port, 
				 DUMMY_DATA_SIZE, dummy_data, // transmit
				 DUMMY_DATA_SIZE, rx_buf1,    // receive
				 NULL); // process
  ck_assert(set_stat == SPIM_TRX_SIMPLE_SET_OK);
  ck_assert(! spim_trx_is_in_transmission((spim_trx*)&trx1));
  
  // queue trx0
  queue_stat = spim_trx_queue((spim_trx*)&trx0);
  ck_assert(queue_stat == SPIM_TRX_QUEUE_OK);
  ck_assert(! spim_trx_is_in_transmission((spim_trx*)&trx0));

  // queue trx1
  queue_stat = spim_trx_queue((spim_trx*)&trx1);
  ck_assert(queue_stat == SPIM_TRX_QUEUE_OK);
  ck_assert(! spim_trx_is_in_transmission((spim_trx*)&trx1));

  // transmit trx0
  int i = 0;
  do {
    if (i > DUMMY_DATA_SIZE + PROC_CALL_MARGIN)
      ck_abort_msg("Transmission timeout");
    process_execute();
    i += 1;
  } while (spim_trx_is_in_transmission((spim_trx*)&trx0));
  
  for (int j = 0; j < DUMMY_DATA_SIZE; ++j) {
    ck_assert(rx_buf0[j] == dummy_data[j]);
    ck_assert(spi_mock_get_last_transmitted_data(DUMMY_DATA_SIZE - j) == 
	      dummy_data[j]);
  }
  ck_assert(spi_mock_get_last_transmitted_data(0) == 0);


  // transmit trx1
  spi_mock_set_incoming_data(dummy_data, 16);
  ck_assert(! spim_trx_is_in_transmission((spim_trx*)&trx1));
  i = 0;
  do {
    if (i > DUMMY_DATA_SIZE + PROC_CALL_MARGIN)
      ck_abort_msg("Transmission timeout");
    process_execute();
    i += 1;
  } while (spim_trx_is_in_transmission((spim_trx*)&trx1));
  
  for (int j = 0; j < DUMMY_DATA_SIZE; ++j) {
    ck_assert(rx_buf1[j] == dummy_data[j]);
    ck_assert(spi_mock_get_last_transmitted_data(DUMMY_DATA_SIZE - j) == 
	      dummy_data[j]);
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

  add_tcase(s, test_trx_set_invalid,          "Trx set invalid");
  add_tcase(s, test_trx_queue_already_queued, "Trx queue already queued");
  add_tcase(s, test_send_single_byte,         "Send single byte");
  add_tcase(s, test_receive_single_byte,      "Receive single byte");
  add_tcase(s, test_send_bytes,               "Send bytes");
  add_tcase(s, test_receive_bytes,            "Receive bytes");
  add_tcase(s, test_send_receive,             "Send/receive bytes");
  add_tcase(s, test_trx_multiple,             "Send/receive multiple trx");

  return s;
}
