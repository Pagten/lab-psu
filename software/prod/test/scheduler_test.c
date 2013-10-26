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
#include "config.h"


static void setup(void)
{
  timer2_mock_init();
  sched_init();
  timer2_mock_ffw_to_oca();
}

static void teardown(void)
{

}

// ****************************************************************************
//                       test_schedule_one_immediately
// ****************************************************************************
static bool task_schedule_one_immediately_executed = false;
static void task_schedule_one_immediately(void* data)
{
  task_schedule_one_immediately_executed = true;
  ck_assert_uint_eq((unsigned int)data, 42);
}


START_TEST(test_schedule_one_immediately)
{
  sched_schedule_status task_scheduled;
  sched_exec_status task_executed;
  unsigned int data = 42;
  task_scheduled = sched_schedule(0, task_schedule_one_immediately, (void*)data);
  ck_assert(task_scheduled == SCHED_OK);
  ck_assert(task_schedule_one_immediately_executed == false);

  task_executed = sched_exec();
  ck_assert(task_executed == SCHED_TASK_EXECUTED);
  ck_assert(task_schedule_one_immediately_executed == true);

  task_executed = sched_exec();
  ck_assert(task_executed == SCHED_IDLE);
}
END_TEST



// ****************************************************************************
//                      test_schedule_multiple_immediately
// ****************************************************************************
static int task_schedule_multiple_immediately_executed = 0;
static void task_schedule_multiple_immediately(void* data)
{
  task_schedule_multiple_immediately_executed += 1;
  ck_assert_int_eq((int)data, task_schedule_multiple_immediately_executed);
}


START_TEST(test_schedule_multiple_immediately)
{
  sched_schedule_status task_scheduled;
  sched_exec_status task_executed;
  
  // Scheduling phase
  task_scheduled = sched_schedule(0, task_schedule_multiple_immediately, (void*)1);
  ck_assert(task_scheduled == SCHED_OK);
  ck_assert(task_schedule_one_immediately_executed == 0);

  task_scheduled = sched_schedule(0, task_schedule_multiple_immediately, (void*)2);
  ck_assert(task_scheduled == SCHED_OK);
  ck_assert(task_schedule_one_immediately_executed == 0);

  task_scheduled = sched_schedule(0, task_schedule_multiple_immediately, (void*)3);
  ck_assert(task_scheduled == SCHED_OK);
  ck_assert(task_schedule_one_immediately_executed == 0);

  // Execution phase
  task_executed = sched_exec();
  ck_assert(task_executed == SCHED_TASK_EXECUTED);
  ck_assert(task_schedule_multiple_immediately_executed == 1);

  task_executed = sched_exec();
  ck_assert(task_executed == SCHED_TASK_EXECUTED);
  ck_assert(task_schedule_multiple_immediately_executed == 2);

  task_executed = sched_exec();
  ck_assert(task_executed == SCHED_TASK_EXECUTED);
  ck_assert(task_schedule_multiple_immediately_executed == 3);

  task_executed = sched_exec();
  ck_assert(task_executed == SCHED_IDLE);
  ck_assert(task_schedule_multiple_immediately_executed == 3);
}
END_TEST


// ****************************************************************************
//                       test_schedule_one_delayed
// ****************************************************************************
static bool task_schedule_one_delayed_executed = false;
static void task_schedule_one_delayed(void* data)
{
  task_schedule_one_delayed_executed = true;
  ck_assert_uint_eq((unsigned int)data, 88);
}


START_TEST(test_schedule_one_delayed)
{
  sched_schedule_status task_scheduled;
  sched_exec_status task_executed;
  unsigned int data = 88;
  task_scheduled = sched_schedule(3, task_schedule_one_delayed, (void*)data);
  ck_assert(task_scheduled == SCHED_OK);
  ck_assert(task_schedule_one_delayed_executed == false);
  
  // TCNT2 = 255
  task_executed = sched_exec();
  ck_assert(task_executed == SCHED_IDLE);
  ck_assert(task_schedule_one_delayed_executed == false);
  timer2_mock_tick();
  
  // TCNT2 = 0
  task_executed = sched_exec();
  ck_assert(task_executed == SCHED_IDLE);
  ck_assert(task_schedule_one_delayed_executed == false);
  timer2_mock_tick();
  
  // TCNT2 = 1
  task_executed = sched_exec();
  ck_assert(task_executed == SCHED_IDLE);
  ck_assert(task_schedule_one_delayed_executed == false);
  timer2_mock_tick();
  
  // TCNT2 = 2
  task_executed = sched_exec();
  ck_assert(task_executed == SCHED_TASK_EXECUTED);
  ck_assert(task_schedule_one_delayed_executed == true);
  timer2_mock_tick();
  
  // TCNT2 = 3
  task_executed = sched_exec();
  ck_assert(task_executed == SCHED_IDLE);
  ck_assert(task_schedule_one_delayed_executed == true);
}
END_TEST


// ****************************************************************************
//                      test_schedule_multiple_delayed
// ****************************************************************************
static int task_schedule_multiple_delayed_executed = 0;
static void task_schedule_multiple_delayed(void* data)
{
  task_schedule_multiple_delayed_executed += 1;
  ck_assert_int_eq((int)data, task_schedule_multiple_delayed_executed);
}


START_TEST(test_schedule_multiple_delayed)
{
  sched_schedule_status task_scheduled;
  sched_exec_status task_executed;
  
  // Scheduling phase
  task_scheduled = sched_schedule(1, task_schedule_multiple_delayed, (void*)1);
  ck_assert(task_scheduled == SCHED_OK);
  ck_assert(task_schedule_one_delayed_executed == 0);

  task_scheduled = sched_schedule(1, task_schedule_multiple_delayed, (void*)2);
  ck_assert(task_scheduled == SCHED_OK);
  ck_assert(task_schedule_one_delayed_executed == 0);

  task_scheduled = sched_schedule(2, task_schedule_multiple_delayed, (void*)3);
  ck_assert(task_scheduled == SCHED_OK);
  ck_assert(task_schedule_one_delayed_executed == 0);

  // Execution phase
  task_executed = sched_exec();
  ck_assert(task_executed == SCHED_IDLE);
  ck_assert(task_schedule_multiple_delayed_executed == 0);

  timer2_mock_tick();
  ck_assert(task_schedule_multiple_delayed_executed == 0);
  task_executed = sched_exec();
  ck_assert(task_executed == SCHED_TASK_EXECUTED);
  ck_assert(task_schedule_multiple_delayed_executed == 1);

  task_executed = sched_exec();
  ck_assert(task_executed == SCHED_TASK_EXECUTED);
  ck_assert(task_schedule_multiple_delayed_executed == 2);

  task_executed = sched_exec();
  ck_assert(task_executed == SCHED_IDLE);
  ck_assert(task_schedule_multiple_delayed_executed == 2);

  timer2_mock_tick();
  ck_assert(task_schedule_multiple_delayed_executed == 2);
  task_executed = sched_exec();
  ck_assert(task_executed == SCHED_TASK_EXECUTED);
  ck_assert(task_schedule_multiple_delayed_executed == 3);

  task_executed = sched_exec();
  ck_assert(task_executed == SCHED_IDLE);
  ck_assert(task_schedule_multiple_delayed_executed == 3);

  timer2_mock_tick();
  task_executed = sched_exec();
  ck_assert(task_executed == SCHED_IDLE);
  ck_assert(task_schedule_multiple_delayed_executed == 3);
}
END_TEST


// ****************************************************************************
//                       test_schedule_max_tasks
// ****************************************************************************
static bool task_schedule_max_tasks_delayed_executed = false;
static void task_schedule_max_tasks_delayed(void* data)
{
  task_schedule_max_tasks_delayed_executed = true;
  ck_assert_uint_eq((unsigned int)data, NULL);
}

START_TEST(test_schedule_max_tasks_delayed)
{
  sched_schedule_status task_scheduled;
  sched_exec_status task_executed;
  int i;
  for (i = 0; i < SCHED_TASKS_MAX; ++i) { 
    task_scheduled = sched_schedule(1, task_schedule_max_tasks_delayed, NULL);
    ck_assert(task_scheduled == SCHED_OK);
    ck_assert(task_schedule_max_tasks_delayed_executed == false);
  }

  task_scheduled = sched_schedule(1, task_schedule_max_tasks_delayed, NULL);
  ck_assert(task_scheduled == SCHED_QUEUE_FULL);
  ck_assert(task_schedule_max_tasks_delayed_executed == false);
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
  Suite *s = suite_create("Scheduler");

  TCase *tc_sched_one_immediately = tcase_create("Schedule one immediately");
  tcase_add_checked_fixture(tc_sched_one_immediately, setup, teardown);
  tcase_add_test(tc_sched_one_immediately, test_schedule_one_immediately);
  suite_add_tcase(s, tc_sched_one_immediately);

  TCase *tc_sched_multiple_immediately = tcase_create("Schedule multiple immediately");
  tcase_add_checked_fixture(tc_sched_multiple_immediately, setup, teardown);
  tcase_add_test(tc_sched_multiple_immediately, test_schedule_multiple_immediately);
  suite_add_tcase(s, tc_sched_multiple_immediately);

  TCase *tc_sched_one_delayed = tcase_create("Schedule one delayed");
  tcase_add_checked_fixture(tc_sched_one_delayed, setup, teardown);
  tcase_add_test(tc_sched_one_delayed, test_schedule_one_delayed);
  suite_add_tcase(s, tc_sched_one_delayed);

  TCase *tc_sched_multiple_delayed = tcase_create("Schedule multiple delayed");
  tcase_add_checked_fixture(tc_sched_multiple_delayed, setup, teardown);
  tcase_add_test(tc_sched_multiple_delayed, test_schedule_multiple_delayed);
  suite_add_tcase(s, tc_sched_multiple_delayed);

  TCase *tc_sched_max_tasks_delayed = tcase_create("Schedule max tasks delayed");
  tcase_add_checked_fixture(tc_sched_max_tasks_delayed, setup, teardown);
  tcase_add_test(tc_sched_max_tasks_delayed, test_schedule_max_tasks_delayed);
  suite_add_tcase(s, tc_sched_max_tasks_delayed);

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

