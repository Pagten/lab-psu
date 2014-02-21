/*
 * mcp4922_test.c
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
 * @file mcp4922_test.c
 * @author Pieter Agten <pieter.agten@gmail.com>
 * @date 21 feb 2014
 *
 * Unit tests for the MCP4922 driver.
 */

#include "mcp4922_test.h"

#include <stdint.h>
#include <check.h>

#include "drivers/mcp4922.h"
#include "hal/spi.h"
#include "util/bit.h"

#define SPI_MOCK_TX_DATA_BUFFER_SIZE 32
#define SPI_DUMMY_PIN 2
#define DUMMY_DAC_VALUE 0x0987
#define EXPECTED_BYTE0_CH_A ((DUMMY_DAC_VALUE >> 8) | _BV(4) | _BV(5))
#define EXPECTED_BYTE0_CH_B ((DUMMY_DAC_VALUE >> 8) | _BV(4) | _BV(5) | _BV(7))
#define EXPECTED_BYTE1_CH_A (DUMMY_DAC_VALUE & 0x00FF)
#define EXPECTED_BYTE1_CH_B (DUMMY_DAC_VALUE & 0x00FF)
#define PROC_CALL_MARGIN 4

static uint8_t dummy_port;

static void setup(void)
{
  spi_mock_init(SPI_MOCK_TX_DATA_BUFFER_SIZE);
  clock_init();
  process_init();
  spim_init();
  mcp4922_init();
  dummy_port = 0xFF;
}

static void teardown(void)
{ }


// ****************************************************************************
//                       test_mcp4922_queue_twice
// ****************************************************************************
START_TEST(test_mcp4922_queue_twice)
{
  mcp4922_pkt pkt;
  mcp4922_pkt_init(&pkt);
  ck_assert(mcp4922_pkt_queue(&pkt) == MCP4922_PKT_QUEUE_OK);
  ck_assert(mcp4922_pkt_queue(&pkt) == MCP4922_PKT_QUEUE_ERROR);
}
END_TEST


// ****************************************************************************
//                       test_mcp4922_send_channel_a
// ****************************************************************************
START_TEST(test_mcp4922_send_channel_a)
{
  mcp4922_pkt pkt;
  mcp4922_pkt_init(&pkt);
  mcp4922_pkt_set(&pkt, SPI_DUMMY_PIN, &dummy_port, 
		  MCP4922_CHANNEL_A, DUMMY_DAC_VALUE);
  ck_assert(! mcp4922_pkt_is_in_transmission(&pkt));

  mcp4922_pkt_queue_status queue_stat = mcp4922_pkt_queue(&pkt);
  ck_assert(queue_stat == MCP4922_PKT_QUEUE_OK);
  ck_assert(! mcp4922_pkt_is_in_transmission(&pkt));


  int i = 0;
  do {
    if (i > PROC_CALL_MARGIN) ck_abort_msg("Transmission timeout");
    process_execute();
    i += 1;
  } while (mcp4922_pkt_is_in_transmission(&pkt));

  ck_assert(spi_mock_get_last_transmitted_data(1) == EXPECTED_BYTE0_CH_A);  
  ck_assert(spi_mock_get_last_transmitted_data(0) == EXPECTED_BYTE1_CH_A);
}
END_TEST


// ****************************************************************************
//                       test_mcp4922_send_channel_b
// ****************************************************************************
START_TEST(test_mcp4922_send_channel_b)
{
  mcp4922_pkt pkt;
  mcp4922_pkt_init(&pkt);
  mcp4922_pkt_set(&pkt, SPI_DUMMY_PIN, &dummy_port, 
		  MCP4922_CHANNEL_B, DUMMY_DAC_VALUE);
  ck_assert(! mcp4922_pkt_is_in_transmission(&pkt));

  mcp4922_pkt_queue_status queue_stat = mcp4922_pkt_queue(&pkt);
  ck_assert(queue_stat == MCP4922_PKT_QUEUE_OK);
  ck_assert(! mcp4922_pkt_is_in_transmission(&pkt));


  int i = 0;
  do {
    if (i > PROC_CALL_MARGIN) ck_abort_msg("Transmission timeout");
    process_execute();
    i += 1;
  } while (mcp4922_pkt_is_in_transmission(&pkt));

  ck_assert(spi_mock_get_last_transmitted_data(1) == EXPECTED_BYTE0_CH_B);  
  ck_assert(spi_mock_get_last_transmitted_data(0) == EXPECTED_BYTE1_CH_B);
}
END_TEST


// ****************************************************************************
//                          test_mcp4922_send_16bit
// ****************************************************************************
START_TEST(test_mcp4922_send_16bit)
{
  mcp4922_pkt pkt;
  mcp4922_pkt_init(&pkt);
  mcp4922_pkt_set(&pkt, SPI_DUMMY_PIN, &dummy_port, 
		  MCP4922_CHANNEL_A, (DUMMY_DAC_VALUE | 0xF000));
  ck_assert(! mcp4922_pkt_is_in_transmission(&pkt));

  mcp4922_pkt_queue_status queue_stat = mcp4922_pkt_queue(&pkt);
  ck_assert(queue_stat == MCP4922_PKT_QUEUE_OK);
  ck_assert(! mcp4922_pkt_is_in_transmission(&pkt));

  int i = 0;
  do {
    if (i > PROC_CALL_MARGIN) ck_abort_msg("Transmission timeout");
    process_execute();
    i += 1;
  } while (mcp4922_pkt_is_in_transmission(&pkt));

  ck_assert(spi_mock_get_last_transmitted_data(1) == EXPECTED_BYTE0_CH_A);  
  ck_assert(spi_mock_get_last_transmitted_data(0) == EXPECTED_BYTE1_CH_A);
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

Suite *mcp4922_suite(void)
{
  Suite *s = suite_create("MCP4922");

  add_tcase(s, test_mcp4922_queue_twice,    "MCP4922 queue twice");
  add_tcase(s, test_mcp4922_send_channel_a, "MCP4922 send on channel A");
  add_tcase(s, test_mcp4922_send_channel_b, "MCP4922 send on channel B");
  add_tcase(s, test_mcp4922_send_16bit,     "MCP4922 send 16-bit");

  return s;
}
