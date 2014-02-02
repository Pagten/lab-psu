/*
 * clock_test.c
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
 * @file clock_test.c
 * @author Pieter Agten <pieter.agten@gmail.com>
 * @date 23 jan 2014
 *
 * Unit test for the clock.
 */

#include "clock_test.h"

#include <check.h>
#include "core/clock.h"
#include "hal/mock_timer.h"

static void setup(void)
{
  clock_init();
}

static void teardown(void)
{

}


// ****************************************************************************
//                       test_no_overflow
// ****************************************************************************
START_TEST(test_no_overflow)
{
  int i;
  for (i = 0; i < 255; ++i) {
    ck_assert_uint_eq(clock_get_time(), i);
    MOCK_TIMER_TICK(CLOCK_TMR);
  }
  ck_assert_uint_eq(clock_get_time(), i);
}
END_TEST


// ****************************************************************************
//                       test_8bit_overflow
// ****************************************************************************
START_TEST(test_8bit_overflow)
{
  int i;
  for (i = 0; i < 65535; ++i) {
    ck_assert_uint_eq(clock_get_time(), i);
    MOCK_TIMER_TICK(CLOCK_TMR);
  }
  ck_assert_uint_eq(clock_get_time(), i);
}
END_TEST


// ****************************************************************************
//                       test_16bit_overflow
// ****************************************************************************
START_TEST(test_16bit_overflow)
{
  int i;
  for (i = 0; i < 65536; ++i) {
    ck_assert_uint_eq(clock_get_time(), i);
    MOCK_TIMER_TICK(CLOCK_TMR);
  }
  ck_assert_uint_eq(clock_get_time(), 0);
}
END_TEST


// ****************************************************************************
//                           Test suite setup
// ****************************************************************************
Suite *clock_suite(void)
{
  Suite *s = suite_create("Clock");

  TCase *tc_no_overflow = tcase_create("No overflow");
  tcase_add_checked_fixture(tc_no_overflow, setup, teardown);
  tcase_add_test(tc_no_overflow, test_no_overflow);
  suite_add_tcase(s, tc_no_overflow);

  TCase *tc_8bit_overflow = tcase_create("8-bit overflow");
  tcase_add_checked_fixture(tc_8bit_overflow, setup, teardown);
  tcase_add_test(tc_8bit_overflow, test_8bit_overflow);
  suite_add_tcase(s, tc_8bit_overflow);

  TCase *tc_16bit_overflow = tcase_create("16-bit overflow");
  tcase_add_checked_fixture(tc_16bit_overflow, setup, teardown);
  tcase_add_test(tc_16bit_overflow, test_16bit_overflow);
  suite_add_tcase(s, tc_16bit_overflow);

  return s;
}
