/*
 * process_test.c
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
 * @file process_test.c
 * @author Pieter Agten <pieter.agten@gmail.com>
 * @date 26 feb 2014
 *
 * Unit tests for the process module.
 */

#include "process_test.h"

#include <stdbool.h>
#include <check.h>
#include "core/process.h"

#define TEST_EVENT       ((process_event_t)0x42)
#define TEST_EVENT_DATA  ((process_data_t)0x88)

PROCESS(test_process);
static bool process_initialized;
static process_event_t last_event;
static process_data_t last_event_data;
static unsigned int loop_counter;

static void setup(void)
{
  process_initialized = false;
  last_event = 0;
  last_event_data = PROCESS_DATA_NULL;
  loop_counter = 0;
}

static void teardown(void)
{ }


// ****************************************************************************
//                           Test process
// ****************************************************************************
PROCESS_THREAD(test_process)
{
  PROCESS_BEGIN();

  process_initialized = true;

  while (true) {
    PROCESS_WAIT_EVENT();
    loop_counter += 1;
    last_event = ev;
    last_event_data = data;
  }

  PROCESS_END();
}

// ****************************************************************************
//                           test_process_start
// ****************************************************************************
START_TEST(test_process_start)
{
  process_start(&test_process);
  ck_assert(process_initialized);
}
END_TEST


// ****************************************************************************
//                           test_process_idle
// ****************************************************************************
START_TEST(test_process_idle)
{
  process_start(&test_process);
  ck_assert(loop_counter == 0);

  process_execute();
  ck_assert(loop_counter == 0);
}
END_TEST


// ****************************************************************************
//                           test_process_post_event
// ****************************************************************************
START_TEST(test_process_post_event)
{
  process_start(&test_process);

  process_post_event(&test_process, TEST_EVENT, TEST_EVENT_DATA);
  ck_assert(loop_counter == 0);

  process_execute();
  ck_assert(loop_counter == 1);
  ck_assert(last_event == TEST_EVENT);
  ck_assert(last_event_data == TEST_EVENT_DATA);

  process_execute();
  ck_assert(loop_counter == 1);
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

Suite *process_suite(void)
{
  Suite *s = suite_create("Process");

  add_tcase(s, test_process_start,      "Start");
  add_tcase(s, test_process_idle,       "Idle");
  add_tcase(s, test_process_post_event, "Post event");

  return s;
}

