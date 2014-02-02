/*
 * timer_test.c
 *
 * Based on the timer implementation of Contiki (http://www.contiki-os.org).
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
 * @file timer_test.c
 * @author Pieter Agten <pieter.agten@gmail.com>
 * @date 2 feb 2014
 *
 * Unit test for the timer.
 */

#include "timer_test.h"

#include <check.h>
#include "core/clock.h"
#include "core/timer.h"

static void setup(void)
{
  clock_init();
}

static void teardown(void)
{

}


// ****************************************************************************
//                       timer_set_normal
// ****************************************************************************
START_TEST(test_timer_set_normal)
{
  const clock_time_t delay = CLOCK_TIME_MAX/2;
  timer t;

  timer_set(&t, delay);
  for (int i = 0; i < delay; ++i) {
    ck_assert(! timer_expired(&t));
    ck_assert_uint_eq(timer_remaining(&t), delay - i);
    MOCK_TIMER_TICK(CLOCK_TMR);
  }

  // Timer should have expired and should remain expired until clock overflow
  for (int i = 0; i < delay; ++i) {
    ck_assert(timer_expired(&t));
    ck_assert_uint_eq(timer_remaining(&t), 0);
    MOCK_TIMER_TICK(CLOCK_TMR);
  }
}
END_TEST


// ****************************************************************************
//                       timer_set_0
// ****************************************************************************
START_TEST(test_timer_set_0)
{
  timer t;
  timer_set(&t, 0);

  ck_assert(timer_expired(&t));
  ck_assert_uint_eq(timer_remaining(&t), 0);
    
  MOCK_TIMER_TICK(CLOCK_TMR);
  ck_assert(timer_expired(&t));
  ck_assert_uint_eq(timer_remaining(&t), 0); 
}
END_TEST


// ****************************************************************************
//                       timer_reset
// ****************************************************************************
START_TEST(test_timer_reset)
{
  const clock_time_t delay = 5;
  const clock_time_t inter_ticks = 3;  
  timer t;

  timer_set(&t, delay);
  for (int i = 0; i < delay; ++i) {
  ck_assert(! timer_expired(&t));
    MOCK_TIMER_TICK(CLOCK_TMR);
  }

  for (int i = 0; i < inter_ticks; ++i) {
    ck_assert(timer_expired(&t));
    MOCK_TIMER_TICK(CLOCK_TMR);
  }
  timer_reset(&t);

  for (int i = 0; i < delay - inter_ticks; ++i) {
    ck_assert(! timer_expired(&t));
    MOCK_TIMER_TICK(CLOCK_TMR);
  }
  ck_assert(timer_expired(&t));
}
END_TEST

// ****************************************************************************
//                       timer_restart
// ****************************************************************************
START_TEST(test_timer_restart)
{
  const clock_time_t delay = 5;
  const clock_time_t inter_ticks = 3;  
  timer t;

  timer_set(&t, delay);
  for (int i = 0; i < delay; ++i) {
  ck_assert(! timer_expired(&t));
    MOCK_TIMER_TICK(CLOCK_TMR);
  }

  for (int i = 0; i < inter_ticks; ++i) {
    ck_assert(timer_expired(&t));
    MOCK_TIMER_TICK(CLOCK_TMR);
  }
  timer_restart(&t);

  for (int i = 0; i < delay; ++i) {
    ck_assert(! timer_expired(&t));
    MOCK_TIMER_TICK(CLOCK_TMR);
  }
  ck_assert(timer_expired(&t));
}
END_TEST


// ****************************************************************************
//                           Test suite setup
// ****************************************************************************
Suite *timer_suite(void)
{
  Suite *s = suite_create("Timer");

  TCase *tc_timer_set_normal = tcase_create("Timer set normal");
  tcase_add_checked_fixture(tc_timer_set_normal, setup, teardown);
  tcase_add_test(tc_timer_set_normal, test_timer_set_normal);
  suite_add_tcase(s, tc_timer_set_normal);

  TCase *tc_timer_set_0 = tcase_create("Timer set 0");
  tcase_add_checked_fixture(tc_timer_set_0, setup, teardown);
  tcase_add_test(tc_timer_set_0, test_timer_set_0);
  suite_add_tcase(s, tc_timer_set_0);

  TCase *tc_timer_reset = tcase_create("Timer reset");
  tcase_add_checked_fixture(tc_timer_reset, setup, teardown);
  tcase_add_test(tc_timer_reset, test_timer_reset);
  suite_add_tcase(s, tc_timer_reset);

  TCase *tc_timer_restart = tcase_create("Timer restart");
  tcase_add_checked_fixture(tc_timer_restart, setup, teardown);
  tcase_add_test(tc_timer_restart, test_timer_restart);
  suite_add_tcase(s, tc_timer_restart);

  return s;
}
