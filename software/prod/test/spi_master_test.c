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

#include "hal/spi.h"
#include "hal/timer2.h"
#include "spi_master.h"
#include "config.h"

#include "spi_master_test.h"

#define SPI_MOCK_TX_DATA_BUFFER_SIZE 32

static uint8_t dummy_port;
static uint8_t dummy_pin_mask;

static void setup(void)
{
  spi_mock_init(SPI_MOCK_TX_DATA_BUFFER_SIZE);
  sched_init();
  spim_init();
  dummy_pin_mask = 1;
  dummy_port = dummy_pin_mask;
}

static void teardown(void)
{

}

// ****************************************************************************
//                       test_send_single_byte
// ****************************************************************************
static uint8_t test_send_single_byte_tx_buf;
static bool test_send_single_byte_cb_executed = false;

static size_t test_send_single_byte_cb(spim_cb_status status, void *trx_cb_data)
{
  ck_assert(status == SPIM_TX_DONE);
  ck_assert((unsigned int)trx_cb_data == 1);
  ck_assert(! (dummy_port & dummy_pin_mask));
  ck_assert(dummy_port == 0);
  ck_assert(spi_mock_get_last_transmitted_data(0) == test_send_single_byte_tx_buf);
  test_send_single_byte_cb_executed = true;
  return 0;
}

START_TEST(test_send_single_byte)
{
  sched_exec_status task_executed;
  test_send_single_byte_tx_buf = 11;
  spim_trx(&test_send_single_byte_tx_buf, 1, 0,
           &dummy_port, dummy_pin_mask, NULL, 0,
           test_send_single_byte_cb, (void*)1U);
  
  do {
    task_executed = sched_exec();
  } while(task_executed == SCHED_TASK_EXECUTED);
  ck_assert(test_send_single_byte_cb_executed);
}
END_TEST

// ****************************************************************************
//                       test_receive_single_byte
// ****************************************************************************
static uint8_t test_receive_single_byte_input;
static uint8_t test_receive_single_byte_rx_buf;
static bool test_receive_single_byte_cb_executed = false;

static size_t test_receive_single_byte_cb(spim_cb_status status, void *trx_cb_data)
{
  ck_assert(status == SPIM_RX_DONE);
  ck_assert((unsigned int)trx_cb_data == 2);
  ck_assert(! (dummy_port & dummy_pin_mask));
  ck_assert(dummy_port == 0);
  ck_assert(test_receive_single_byte_rx_buf == 22);
  test_receive_single_byte_cb_executed = true;
  return 0;
}

START_TEST(test_receive_single_byte)
{
  sched_exec_status task_executed;
  test_receive_single_byte_input = 22;
  spi_mock_set_incoming_data(&test_receive_single_byte_input, 1);
  spim_trx(NULL, 0, 0,
           &dummy_port, dummy_pin_mask, &test_receive_single_byte_rx_buf, 1,
           test_receive_single_byte_cb, (void*)2U);
  do {
    task_executed = sched_exec();
  } while(task_executed == SCHED_TASK_EXECUTED);
  ck_assert(test_receive_single_byte_cb_executed);
}
END_TEST


// ****************************************************************************
//                       test_send_bytes
// ****************************************************************************
#define TEST_SEND_BYTES_LEN 16
static uint8_t test_send_bytes_tx_buf[TEST_SEND_BYTES_LEN];
static bool test_send_bytes_cb_executed = false;

static size_t test_send_bytes_cb(spim_cb_status status, void *trx_cb_data)
{
  ck_assert(status == SPIM_TX_DONE);
  ck_assert((unsigned int)trx_cb_data == 3);
  ck_assert(! (dummy_port & dummy_pin_mask));
  ck_assert(dummy_port == 0);
  unsigned int i;
  for (i = 0; i < TEST_SEND_BYTES_LEN; ++i) {
    ck_assert(spi_mock_get_last_transmitted_data(i) == test_send_bytes_tx_buf[TEST_SEND_BYTES_LEN-i-1]);
  }
  test_send_bytes_cb_executed = true;
  return 0;
}

START_TEST(test_send_bytes)
{ 
  sched_exec_status task_executed;
  unsigned int i;
  for (i = 0; i < TEST_SEND_BYTES_LEN; ++i) {
    test_send_bytes_tx_buf[i] = (uint8_t)i;
  }
  spim_trx(test_send_bytes_tx_buf, TEST_SEND_BYTES_LEN, 0,
           &dummy_port, dummy_pin_mask, NULL, 0,
           test_send_bytes_cb, (void*)3U);
  do {
    task_executed = sched_exec();
  } while(task_executed == SCHED_TASK_EXECUTED);
  ck_assert(test_send_bytes_cb_executed);
}
END_TEST


// ****************************************************************************
//                       test_receive_bytes
// ****************************************************************************
#define TEST_RECEIVE_BYTES_LEN 16
static uint8_t test_receive_bytes_rx_buf[TEST_RECEIVE_BYTES_LEN];
static uint8_t test_receive_bytes_mock[TEST_RECEIVE_BYTES_LEN];
static bool test_receive_bytes_cb_executed = false;

static size_t test_receive_bytes_cb(spim_cb_status status, void *trx_cb_data)
{
  ck_assert(status == SPIM_RX_DONE);
  ck_assert((unsigned int)trx_cb_data == 4);
  ck_assert(! (dummy_port & dummy_pin_mask));
  ck_assert(dummy_port == 0);
  unsigned int i;
  for (i = 0; i < TEST_RECEIVE_BYTES_LEN; ++i) {
    ck_assert(test_receive_bytes_mock[i] == test_receive_bytes_rx_buf[i]);
  }
  test_receive_bytes_cb_executed = true;
  return 0;
}

START_TEST(test_receive_bytes)
{
  sched_exec_status task_executed;
  unsigned int i;
  for (i = 0; i < TEST_RECEIVE_BYTES_LEN; ++i) {
    test_receive_bytes_mock[i] = (uint8_t)i;
  }
  spi_mock_set_incoming_data(test_receive_bytes_mock, TEST_RECEIVE_BYTES_LEN);
  spim_trx(NULL, 0, 0,
           &dummy_port, dummy_pin_mask,
	   test_receive_bytes_rx_buf, TEST_RECEIVE_BYTES_LEN,
           test_receive_bytes_cb, (void*)4U);
  do {
    task_executed = sched_exec();
  } while(task_executed == SCHED_TASK_EXECUTED);
 
  ck_assert(test_receive_bytes_cb_executed);
}
END_TEST

// ****************************************************************************
//                       test_send_receive
// ****************************************************************************
#define TEST_SEND_RECEIVE_TX_LEN 20
#define TEST_SEND_RECEIVE_RX_LEN 10
static uint8_t test_send_receive_tx_buf[TEST_SEND_RECEIVE_TX_LEN];
static uint8_t test_send_receive_rx_buf[TEST_SEND_RECEIVE_RX_LEN];
static uint8_t test_send_receive_mock[TEST_SEND_RECEIVE_RX_LEN];
static int test_send_receive_cb_executed = 0;

static size_t test_send_receive_cb(spim_cb_status status, void *trx_cb_data)
{
   unsigned int i;
   switch(test_send_receive_cb_executed) {
   case 0:
     ck_assert(status == SPIM_RX_DONE);
     for (i = 0; i < TEST_SEND_RECEIVE_RX_LEN; ++i) {
       ck_assert(test_send_receive_mock[i] == test_send_receive_rx_buf[i]);
     }
     break;
   case 1:
     ck_assert(status == SPIM_TX_DONE);
     for (i = 0; i < TEST_SEND_RECEIVE_RX_LEN; ++i) {
      ck_assert(spi_mock_get_last_transmitted_data(i) == test_send_receive_tx_buf[TEST_SEND_RECEIVE_TX_LEN-i-1]);
     }
     break;
   default:
     ck_abort_msg("Unexpected value for test_send_receive_cb_executed");
     break;
  }
  ck_assert((unsigned int)trx_cb_data == 5);
  ck_assert(! (dummy_port & dummy_pin_mask));
  ck_assert(dummy_port == 0);
  test_send_receive_cb_executed += 1;
  return 0;
}

START_TEST(test_send_receive)
{
  sched_exec_status task_executed;
  unsigned int i;
  for (i = 0; i < TEST_SEND_RECEIVE_TX_LEN; ++i) {
    test_send_receive_tx_buf[i] = (uint8_t)i;
  }
  for (i = 0; i < TEST_SEND_RECEIVE_RX_LEN; ++i) {
    test_send_receive_mock[i] = (uint8_t)i;
  }
  spi_mock_set_incoming_data(test_send_receive_mock, TEST_SEND_RECEIVE_RX_LEN);
  spim_trx(test_send_receive_tx_buf, TEST_SEND_RECEIVE_TX_LEN, 0,
           &dummy_port, dummy_pin_mask,
	   test_send_receive_rx_buf, TEST_SEND_RECEIVE_RX_LEN,
           test_send_receive_cb, (void*)5U);
  do {
    task_executed = sched_exec();
  } while(task_executed == SCHED_TASK_EXECUTED);
 
  ck_assert(test_send_receive_cb_executed == 2);
}
END_TEST


// ****************************************************************************
//                       test_receive_bytes_cont
// ****************************************************************************
#define TEST_RECEIVE_BYTES_CONT_LEN 16
#define TEST_RECEIVE_BYTES_CONT_COUNT 4
static uint8_t test_receive_bytes_cont_rx_buf[TEST_RECEIVE_BYTES_CONT_LEN * TEST_RECEIVE_BYTES_CONT_COUNT];
static uint8_t test_receive_bytes_cont_mock[TEST_RECEIVE_BYTES_CONT_LEN * TEST_RECEIVE_BYTES_CONT_COUNT];
static int test_receive_bytes_cont_cb_executed = 0;

static size_t test_receive_bytes_cont_cb(spim_cb_status status, void *trx_cb_data)
{
  ck_assert(status == SPIM_RX_DONE);
  ck_assert((unsigned int)trx_cb_data == 6);
  ck_assert(! (dummy_port & dummy_pin_mask));
  ck_assert(dummy_port == 0);

  test_receive_bytes_cont_cb_executed += 1;
  if (test_receive_bytes_cont_cb_executed <= 0 ||
      test_receive_bytes_cont_cb_executed > TEST_RECEIVE_BYTES_CONT_COUNT) {
    ck_abort_msg("Unexpected value for test_receive_bytes_cont_cb_executed");
  }

  unsigned int i;
  for (i = 0; i < TEST_RECEIVE_BYTES_CONT_LEN * test_receive_bytes_cont_cb_executed; ++i) {
    ck_assert(test_receive_bytes_cont_mock[i] == test_receive_bytes_cont_rx_buf[i]);
  }
  if (test_receive_bytes_cont_cb_executed < TEST_RECEIVE_BYTES_CONT_COUNT) {
    return TEST_RECEIVE_BYTES_CONT_LEN;
  } else {
    return 0;
  }
}

START_TEST(test_receive_bytes_cont)
{
  sched_exec_status task_executed;
  unsigned int i;
  for (i = 0; i < TEST_RECEIVE_BYTES_CONT_LEN * TEST_RECEIVE_BYTES_CONT_COUNT; ++i) {
    test_receive_bytes_cont_mock[i] = (uint8_t)i;
  }
  spi_mock_set_incoming_data(test_receive_bytes_cont_mock, TEST_RECEIVE_BYTES_CONT_LEN * TEST_RECEIVE_BYTES_CONT_COUNT);
  spim_trx(NULL, 0, 0,
           &dummy_port, dummy_pin_mask,
	   test_receive_bytes_cont_rx_buf, TEST_RECEIVE_BYTES_CONT_LEN,
           test_receive_bytes_cont_cb, (void*)6U);
  do {
    task_executed = sched_exec();
  } while(task_executed == SCHED_TASK_EXECUTED);
 
  ck_assert(test_receive_bytes_cont_cb_executed == 4);
}
END_TEST

// ****************************************************************************
//                       test_trx_multiple
// ****************************************************************************
#define TEST_TRX_MULTIPLE_TX_LEN0 1
#define TEST_TRX_MULTIPLE_TX_LEN1 2
#define TEST_TRX_MULTIPLE_RX_LEN0 3
#define TEST_TRX_MULTIPLE_RX_LEN1 4
static uint8_t test_trx_multiple_tx_buf[TEST_TRX_MULTIPLE_TX_LEN0+TEST_TRX_MULTIPLE_TX_LEN1];
static uint8_t test_trx_multiple_rx_buf[TEST_TRX_MULTIPLE_RX_LEN0+TEST_TRX_MULTIPLE_RX_LEN1];
static uint8_t test_trx_multiple_mock[TEST_TRX_MULTIPLE_RX_LEN0+TEST_TRX_MULTIPLE_RX_LEN1];
static bool test_trx_multiple_cb0_rx_executed = false;
static bool test_trx_multiple_cb0_tx_executed = false;
static bool test_trx_multiple_cb1_rx_executed = false;
static bool test_trx_multiple_cb1_tx_executed = false;

static size_t test_trx_multiple_cb0(spim_cb_status status, void *trx_cb_data)
{
  ck_assert((unsigned int)trx_cb_data == 6);
  ck_assert(! (dummy_port & dummy_pin_mask));
  ck_assert(dummy_port == 0);
  unsigned int i;
  if (status == SPIM_RX_DONE) {
    for (i = 0; i < TEST_TRX_MULTIPLE_RX_LEN0; ++i) {
      ck_assert(test_trx_multiple_mock[i] == test_trx_multiple_rx_buf[i]);
    }
    test_trx_multiple_cb0_rx_executed = true;
  } else if (status = SPIM_TX_DONE) {
    for (i = 0; i < TEST_TRX_MULTIPLE_TX_LEN0; ++i) {
      ck_assert(spi_mock_get_last_transmitted_data(i) == test_trx_multiple_tx_buf[TEST_TRX_MULTIPLE_TX_LEN0-i-1]);
    }
    test_trx_multiple_cb0_tx_executed = true;
  }
  return 0;
}
static size_t test_trx_multiple_cb1(spim_cb_status status, void *trx_cb_data)
{
  ck_assert((unsigned int)trx_cb_data == 7);
  ck_assert(! (dummy_port & dummy_pin_mask));
  ck_assert(dummy_port == 0);
  unsigned int i;
  if (status == SPIM_RX_DONE) {
    for (i = 0; i < TEST_TRX_MULTIPLE_RX_LEN0+TEST_TRX_MULTIPLE_RX_LEN1; ++i) {
      ck_assert(test_trx_multiple_mock[i] == test_trx_multiple_rx_buf[i]);
    }
    test_trx_multiple_cb1_rx_executed = true;
  } else if (status = SPIM_TX_DONE) {
    for (i = 0; i < TEST_TRX_MULTIPLE_TX_LEN0+TEST_TRX_MULTIPLE_TX_LEN1; ++i) {
      ck_assert(spi_mock_get_last_transmitted_data(i) == test_trx_multiple_tx_buf[TEST_TRX_MULTIPLE_TX_LEN0+TEST_TRX_MULTIPLE_TX_LEN1-i-1]);
    } 
    test_trx_multiple_cb1_tx_executed = true;
  }
  return 0;
}

START_TEST(test_trx_multiple)
{
  sched_exec_status task_executed;
  unsigned int i;
  for (i = 0; i < TEST_TRX_MULTIPLE_TX_LEN0+TEST_TRX_MULTIPLE_TX_LEN1; ++i) {
    test_trx_multiple_tx_buf[i] = (uint8_t)i;
  }
  for (i = 0; i < TEST_TRX_MULTIPLE_RX_LEN0+TEST_TRX_MULTIPLE_RX_LEN1; ++i) {
    test_trx_multiple_mock[i] = (uint8_t)i;
  }
  spi_mock_set_incoming_data(test_trx_multiple_mock, TEST_TRX_MULTIPLE_RX_LEN0+TEST_TRX_MULTIPLE_RX_LEN1);
  spim_trx(test_trx_multiple_tx_buf, TEST_TRX_MULTIPLE_TX_LEN0, 0,
           &dummy_port, dummy_pin_mask,
	   test_trx_multiple_rx_buf, TEST_TRX_MULTIPLE_RX_LEN0,
           test_trx_multiple_cb0, (void*)6U);

  spim_trx(&test_trx_multiple_tx_buf[TEST_TRX_MULTIPLE_TX_LEN0], 
           TEST_TRX_MULTIPLE_TX_LEN1, 0,
           &dummy_port, dummy_pin_mask,
	   &test_trx_multiple_rx_buf[TEST_TRX_MULTIPLE_RX_LEN0],
           TEST_TRX_MULTIPLE_RX_LEN1,
           test_trx_multiple_cb1, (void*)7U);
  do {
    task_executed = sched_exec();
  } while(!test_trx_multiple_cb0_rx_executed && task_executed == SCHED_TASK_EXECUTED);
  sched_exec();
  ck_assert(dummy_port & dummy_pin_mask);
  do {
    task_executed = sched_exec();
  } while( task_executed == SCHED_TASK_EXECUTED);
  
  ck_assert(test_trx_multiple_cb0_tx_executed);
  ck_assert(test_trx_multiple_cb0_rx_executed);
  ck_assert(test_trx_multiple_cb1_tx_executed);
  ck_assert(test_trx_multiple_cb1_rx_executed);
}
END_TEST

// ****************************************************************************
//                       test_trx_delay
// ****************************************************************************
#define TEST_TRX_DELAY_TRX_LEN 10
#define TEST_TRX_DELAY_DELAY 200
static uint8_t test_trx_delay_tx_buf[TEST_TRX_DELAY_TRX_LEN];
static uint8_t test_trx_delay_rx_buf[TEST_TRX_DELAY_TRX_LEN];
static uint8_t test_trx_delay_mock[TEST_TRX_DELAY_TRX_LEN];
static int test_trx_delay_cb_executed = 0;

static size_t test_trx_delay_cb(spim_cb_status status, void *trx_cb_data)
{
   unsigned int i;
   switch(status) {
   case SPIM_RX_DONE:
     for (i = 0; i < TEST_TRX_DELAY_TRX_LEN; ++i) {
       ck_assert(test_trx_delay_mock[i] == test_trx_delay_rx_buf[i]);
     }
     break;
   case SPIM_TX_DONE:
     for (i = 0; i < TEST_TRX_DELAY_TRX_LEN; ++i) {
      ck_assert(spi_mock_get_last_transmitted_data(i) == test_trx_delay_tx_buf[TEST_TRX_DELAY_TRX_LEN-i-1]);
     }
     break;
   default:
     ck_abort_msg("Unexpected status in test_trx_delay_cb");
     break;
  }
  ck_assert((unsigned int)trx_cb_data == 8);
  ck_assert(! (dummy_port & dummy_pin_mask));
  ck_assert(dummy_port == 0);
  test_trx_delay_cb_executed += 1;
  return 0;
}

START_TEST(test_trx_delay)
{
  sched_exec_status task_executed;
  unsigned int i;
  for (i = 0; i < TEST_TRX_DELAY_TRX_LEN; ++i) {
    test_trx_delay_tx_buf[i] = (uint8_t)i;
  }
  for (i = 0; i < TEST_TRX_DELAY_TRX_LEN; ++i) {
    test_trx_delay_mock[i] = (uint8_t)i;
  }
  spi_mock_set_incoming_data(test_trx_delay_mock, TEST_TRX_DELAY_TRX_LEN);
  spim_trx(test_trx_delay_tx_buf, TEST_TRX_DELAY_TRX_LEN, TEST_TRX_DELAY_DELAY,
           &dummy_port, dummy_pin_mask,
	   test_trx_delay_rx_buf, TEST_TRX_DELAY_TRX_LEN,
           test_trx_delay_cb, (void*)8U);

  unsigned int bytes_trxed = 0;
  for (bytes_trxed = 0; bytes_trxed < TEST_TRX_DELAY_TRX_LEN; ++bytes_trxed) {
    do {
      task_executed = sched_exec();
    } while(task_executed == SCHED_TASK_EXECUTED);
    
    ck_assert(test_send_receive_cb_executed == 0);
    ck_assert(spi_mock_get_last_transmitted_data(0) == test_trx_delay_tx_buf[bytes_trxed]);
    for (i = 0; i < bytes_trxed; ++i) {
      ck_assert(test_trx_delay_mock[i] == test_trx_delay_rx_buf[i]);
    }

    timer2_mock_ffw_to_oca();
  }
  do {
      task_executed = sched_exec();
    } while(task_executed == SCHED_TASK_EXECUTED);
  ck_assert(test_trx_delay_cb_executed == 2);
}
END_TEST



// ****************************************************************************
//                           Test suite setup
// ****************************************************************************
Suite *spi_master_suite(void)
{
  Suite *s = suite_create("Spi master");

  TCase *tc_send_single_byte = tcase_create("Send single byte");
  tcase_add_checked_fixture(tc_send_single_byte, setup, teardown);
  tcase_add_test(tc_send_single_byte, test_send_single_byte);
  suite_add_tcase(s, tc_send_single_byte);

  TCase *tc_receive_single_byte = tcase_create("Receive single byte");
  tcase_add_checked_fixture(tc_receive_single_byte, setup, teardown);
  tcase_add_test(tc_receive_single_byte, test_receive_single_byte);
  suite_add_tcase(s, tc_receive_single_byte);

  TCase *tc_send_bytes = tcase_create("Send bytes");
  tcase_add_checked_fixture(tc_send_bytes, setup, teardown);
  tcase_add_test(tc_send_bytes, test_send_bytes);
  suite_add_tcase(s, tc_send_bytes);

  TCase *tc_receive_bytes = tcase_create("Receive bytes");
  tcase_add_checked_fixture(tc_receive_bytes, setup, teardown);
  tcase_add_test(tc_receive_bytes, test_receive_bytes);
  suite_add_tcase(s, tc_receive_bytes);

  TCase *tc_send_receive = tcase_create("Send/receive");
  tcase_add_checked_fixture(tc_send_receive, setup, teardown);
  tcase_add_test(tc_send_receive, test_send_receive);
  suite_add_tcase(s, tc_send_receive);

  TCase *tc_receive_bytes_cont = tcase_create("Receive bytes cont");
  tcase_add_checked_fixture(tc_receive_bytes_cont, setup, teardown);
  tcase_add_test(tc_receive_bytes_cont, test_receive_bytes_cont);
  suite_add_tcase(s, tc_receive_bytes_cont);

  TCase *tc_trx_multiple = tcase_create("Trx multiple");
  tcase_add_checked_fixture(tc_trx_multiple, setup, teardown);
  tcase_add_test(tc_trx_multiple, test_trx_multiple);
  suite_add_tcase(s, tc_trx_multiple);

  TCase *tc_trx_delay = tcase_create("Trx delay");
  tcase_add_checked_fixture(tc_trx_delay, setup, teardown);
  tcase_add_test(tc_trx_delay, test_trx_delay);
  suite_add_tcase(s, tc_trx_delay);

  return s;
}
