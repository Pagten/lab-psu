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
#include "spi_master.h"
#include "config.h"

#include "spi_master_test.h"

static uint8_t dummy_port;
static uint8_t dummy_pin_mask;

static void setup(void)
{
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

static size_t test_send_single_byte_cb(spim_cb_status status, void *rx_cb_data)
{
  ck_assert(status == SPIM_TX_DONE);
  ck_assert((unsigned int)rx_cb_data == 88);
  ck_assert(! (dummy_port & dummy_pin_mask));
  ck_assert(spi_mock.data_reg == test_send_single_byte_tx_buf);
  return 0;
}

START_TEST(test_send_single_byte)
{
  test_send_single_byte_tx_buf = 42;
  spim_trx(&test_send_single_byte_tx_buf, sizeof(test_send_single_byte_tx_buf), 0,
           &dummy_port, dummy_pin_mask, NULL, 0,
           test_send_single_byte_cb, (void*)88U);
}
END_TEST

Suite *spi_master_suite(void)
{
  Suite *s = suite_create("Spi master");

  TCase *tc_send_single_byte = tcase_create("Send single byte");
  tcase_add_checked_fixture(tc_send_single_byte, setup, teardown);
  tcase_add_test(tc_send_single_byte, test_send_single_byte);
  suite_add_tcase(s, tc_send_single_byte);

  return s;
}
