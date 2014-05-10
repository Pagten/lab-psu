/*
 * rotary_test.c
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
 * @file rotary_test.c
 * @author Pieter Agten <pieter.agten@gmail.com>
 * @date 8 jan 2014
 *
 * Unit test for the rotary encoder routines.
 */

#include <check.h>
#include <stdbool.h>

#include "core/rotary.h"
#include "rotary_test.h"


static void setup(void)
{
}

static void teardown(void)
{
}

// ****************************************************************************
//                               test_rotary_input
// ****************************************************************************
static void test_rot_input_values(uint8_t bit_a, uint8_t bit_b)
{
  for (int a = 0; a <= 1; ++a) {
    for (int b = 0; b <= 1; ++b) {
      uint8_t input = (a << bit_a) | (b << bit_b);
      uint8_t output = (b << 1) | a;
      ck_assert_uint_eq(rot_input(input, bit_a, bit_b), output);
    }
  }
}

START_TEST(test_rotary_input)
{
  for (uint8_t bit_a = 0; bit_a < 8; ++bit_a) {
    for (uint8_t bit_b = 0; bit_b < 8; ++bit_b) {
      if (bit_a == bit_b) {
	continue;
      }
      test_rot_input_values(bit_a, bit_b);      
    }
  }
}
END_TEST


// ****************************************************************************
//                               test_rotary_cw0
// ****************************************************************************
START_TEST(test_rotary_cw0)
{
  int input;
  rotary rot;
  rot_step_status result;
  rot_init(&rot);
  
  input = 0b00;
  result = rot_process_step(&rot, input);
  ck_assert(result == ROT_NO_STEP);

  input = 0b01;
  result = rot_process_step(&rot, input);
  ck_assert(result == ROT_NO_STEP);

  input = 0b11;
  result = rot_process_step(&rot, input);
  ck_assert(result == ROT_STEP_CW);
}
END_TEST

// ****************************************************************************
//                               test_rotary_cw1
// ****************************************************************************
START_TEST(test_rotary_cw1)
{
  int input;
  rotary rot;
  rot_step_status result;
  rot_init(&rot);

  input = 0b11;
  result = rot_process_step(&rot, input);
  ck_assert(result == ROT_NO_STEP);

  input = 0b10;
  result = rot_process_step(&rot, input);
  ck_assert(result == ROT_NO_STEP);

  input = 0b00;
  result = rot_process_step(&rot, input);
  ck_assert(result == ROT_STEP_CW);
}
END_TEST

// ****************************************************************************
//                               test_rotary_cw_full
// ****************************************************************************
START_TEST(test_rotary_cw_full)
{
  int input;
  rotary rot;
  rot_step_status result;
  rot_init(&rot);

  input = 0b00;
  result = rot_process_step(&rot, input);
  ck_assert(result == ROT_NO_STEP);
  
  input = 0b01;
  result = rot_process_step(&rot, input);
  ck_assert(result == ROT_NO_STEP);
  
  input = 0b11;
  result = rot_process_step(&rot, input);
  ck_assert(result == ROT_STEP_CW);

  input = 0b10;
  result = rot_process_step(&rot, input);
  ck_assert(result == ROT_NO_STEP);

  input = 0b00;
  result = rot_process_step(&rot, input);
  ck_assert(result == ROT_STEP_CW);
}
END_TEST


// ****************************************************************************
//                               test_rotary_ccw0
// ****************************************************************************
START_TEST(test_rotary_ccw0)
{
  int input;
  rotary rot;
  rot_step_status result;
  rot_init(&rot);

  input = 0b00;
  result = rot_process_step(&rot, input);
  ck_assert(result == ROT_NO_STEP);

  input = 0b10;
  result = rot_process_step(&rot, input);
  ck_assert(result == ROT_NO_STEP);

  input = 0b11;
  result = rot_process_step(&rot, input);
  ck_assert(result == ROT_STEP_CCW);
}
END_TEST

// ****************************************************************************
//                               test_rotary_ccw1
// ****************************************************************************
START_TEST(test_rotary_ccw1)
{
  int input;
  rotary rot;
  rot_step_status result;
  rot_init(&rot);

  input = 0b11;
  result = rot_process_step(&rot, input);
  ck_assert(result == ROT_NO_STEP);

  input = 0b01;
  result = rot_process_step(&rot, input);
  ck_assert(result == ROT_NO_STEP);

  input = 0b00;
  result = rot_process_step(&rot, input);
  ck_assert(result == ROT_STEP_CCW);
}
END_TEST

// ****************************************************************************
//                               test_rotary_ccw_full
// ****************************************************************************
START_TEST(test_rotary_ccw_full)
{
  int input;
  rotary rot;
  rot_step_status result;
  rot_init(&rot);

  input = 0b00;
  result = rot_process_step(&rot, input);
  ck_assert(result == ROT_NO_STEP);
  
  input = 0b10;
  result = rot_process_step(&rot, input);
  ck_assert(result == ROT_NO_STEP);
  
  input = 0b11;
  result = rot_process_step(&rot, input);
  ck_assert(result == ROT_STEP_CCW);

  input = 0b01;
  result = rot_process_step(&rot, input);
  ck_assert(result == ROT_NO_STEP);

  input = 0b00;
  result = rot_process_step(&rot, input);
  ck_assert(result == ROT_STEP_CCW);
}
END_TEST



// ****************************************************************************
//                           Test suite setup
// ****************************************************************************
Suite *rotary_suite(void)
{
  Suite *s = suite_create("Rotary");

  TCase *tc_rotary_input = tcase_create("Rotary input");
  tcase_add_checked_fixture(tc_rotary_input, setup, teardown);
  tcase_add_test(tc_rotary_input, test_rotary_input);
  suite_add_tcase(s, tc_rotary_input);

  TCase *tc_rotary_cw0 = tcase_create("Rotary cw0");
  tcase_add_checked_fixture(tc_rotary_cw0, setup, teardown);
  tcase_add_test(tc_rotary_cw0, test_rotary_cw0);
  suite_add_tcase(s, tc_rotary_cw0);

  TCase *tc_rotary_cw1 = tcase_create("Rotary cw1");
  tcase_add_checked_fixture(tc_rotary_cw1, setup, teardown);
  tcase_add_test(tc_rotary_cw1, test_rotary_cw1);
  suite_add_tcase(s, tc_rotary_cw1);

  TCase *tc_rotary_cw_full = tcase_create("Rotary cw full");
  tcase_add_checked_fixture(tc_rotary_cw_full, setup, teardown);
  tcase_add_test(tc_rotary_cw_full, test_rotary_cw_full);
  suite_add_tcase(s, tc_rotary_cw_full);

  TCase *tc_rotary_ccw0 = tcase_create("Rotary ccw0");
  tcase_add_checked_fixture(tc_rotary_ccw0, setup, teardown);
  tcase_add_test(tc_rotary_ccw0, test_rotary_ccw0);
  suite_add_tcase(s, tc_rotary_ccw0);

  TCase *tc_rotary_ccw1 = tcase_create("Rotary ccw1");
  tcase_add_checked_fixture(tc_rotary_ccw1, setup, teardown);
  tcase_add_test(tc_rotary_ccw1, test_rotary_ccw1);
  suite_add_tcase(s, tc_rotary_ccw1);

  TCase *tc_rotary_ccw_full = tcase_create("Rotary ccw full");
  tcase_add_checked_fixture(tc_rotary_ccw_full, setup, teardown);
  tcase_add_test(tc_rotary_ccw_full, test_rotary_ccw_full);
  suite_add_tcase(s, tc_rotary_ccw_full);

  return s;
}
