/*
 * scheduler_test.c
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
 * @file scheduler_test.c
 * @author Pieter Agten <pieter.agten@gmail.com>
 * @date 21 jul 2013
 *
 * Unit test for the scheduler.
 */

#include <stdbool.h>
#include <stdlib.h>
#include <check.h>

#include <hal/timer2.h>
#include "scheduler.h"


static void setup(void)
{
  timer2_mock_init();
  sched_init();
  timer2_mock_ffw_to_oca();
}

static void teardown(void)
{

}

// ***** test_schedule_task0_immediately *****
static bool task_schedule_immediately0_ran = false;
static void task_schedule_immediately0(void* data)
{
  task_schedule_immediately0_ran = true;
  ck_assert_uint_eq((unsigned int)data, 42);
}

static bool task_schedule_immediately1_ran = false;
static void task_schedule_immediately1(void* data)
{
  task_schedule_immediately1_ran = true;
  ck_assert(task_schedule_immediately0_ran == true);
  ck_assert_ptr_eq(data, NULL);
}


START_TEST(test_schedule_immediately0)
{
  sched_exec_status_t task_executed;
  unsigned int data = 42;
  sched_schedule(0, task_schedule_immediately0, (void*)data);
  sched_schedule(0, task_schedule_immediately1, NULL);
  ck_assert(task_schedule_immediately0_ran == false);
  ck_assert(task_schedule_immediately1_ran == false);
  task_executed = sched_exec();
  ck_assert(task_executed == SCHED_TASK_EXECUTED);
  ck_assert(task_schedule_immediately0_ran == true);
  ck_assert(task_schedule_immediately1_ran == false);
  task_executed = sched_exec();
  ck_assert(task_executed == SCHED_TASK_EXECUTED);
  ck_assert(task_schedule_immediately0_ran == true);
  ck_assert(task_schedule_immediately1_ran == true);
  task_executed = sched_exec();
  ck_assert(task_executed == SCHED_IDLE);
}
END_TEST



static bool task_schedule_delayed0_ran = false;
static void task_schedule_delayed0(void* data)
{
  task_schedule_delayed0_ran = true;
  ck_assert_uint_eq((unsigned int)data, 88);
}


START_TEST(test_schedule_delayed0)
{
  sched_exec_status_t task_executed;
  unsigned int data = 88;
  sched_schedule(3, task_schedule_delayed0, (void*)data);
  ck_assert(task_schedule_delayed0_ran == false);
  
  // TCNT2 = 255
  task_executed = sched_exec();
  ck_assert(task_executed == SCHED_IDLE);
  ck_assert(task_schedule_delayed0_ran == false);
  timer2_mock_tick();
  
  // TCNT2 = 0
  task_executed = sched_exec();
  ck_assert(task_executed == SCHED_IDLE);
  ck_assert(task_schedule_delayed0_ran == false);
  timer2_mock_tick();
  
  // TCNT2 = 1
  task_executed = sched_exec();
  ck_assert(task_executed == SCHED_IDLE);
  ck_assert(task_schedule_delayed0_ran == false);
  timer2_mock_tick();
  
  // TCNT2 = 2
  task_executed = sched_exec();
  ck_assert(task_executed == SCHED_TASK_EXECUTED);
  ck_assert(task_schedule_delayed0_ran == true);
  timer2_mock_tick();
  
  // TCNT2 = 3
  task_executed = sched_exec();
  ck_assert(task_executed == SCHED_IDLE);
  ck_assert(task_schedule_delayed0_ran == true);
}
END_TEST


// ***** test_schedule_recursive *****
/*#define NB_RECURSIVE_SCHEDS SCHED_TASK_QUEUE_SIZE//todo: this is not public
static void task_schedule_recursive(void* data)
{
  // ASSERT (uint16_t)data <= 
  // SUCCESS
}

START_TEST(test_schedule_task0_immediately)
{
  uint16_t data = 88;
  sched_schedule(0, task_schedule_recursive, (void*)data);
  // ASSERT task0_ran = false
  sched_start();
  ck_abort_msg("sched_start shouldn't return");
  }*/

Suite *schedule_suite(void)
{
  Suite *s = suite_create("Schedule function");

  TCase *tc_sched_immediately0 = tcase_create("Schedule immediately 0");
  tcase_add_checked_fixture(tc_sched_immediately0, setup, teardown);
  tcase_add_test(tc_sched_immediately0, test_schedule_immediately0);
  suite_add_tcase(s, tc_sched_immediately0);

  TCase *tc_sched_delayed0 = tcase_create("Schedule delayed 0");
  tcase_add_checked_fixture(tc_sched_delayed0, setup, teardown);
  tcase_add_test(tc_sched_delayed0, test_schedule_delayed0);
  suite_add_tcase(s, tc_sched_delayed0);

  return s;
}

int main(void)
{
  int number_failed;
  Suite *s = schedule_suite();
  SRunner *sr = srunner_create(s);
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

