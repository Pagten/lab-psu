/*
 * pwlf_test.c
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
 * @file pwlf_test.c
 * @author Pieter Agten <pieter.agten@gmail.com>
 * @date 13 Oct 2014
 *
 * Unit test for the piecewise linear function module.
 */
#include "pwlf_test.h"

#include <check.h>
#include <stdbool.h>
#include <stdio.h>

#include "core/pwlf.h"
#include "util/math.h"

static void setup(void)
{
}

static void teardown(void)
{
}


static float
expected(float x, float x1, float x2, float y1, float y2)
{
  return y1 + (x - x1) * (y2 - y1)/(x2 - x1);
}

// ****************************************************************************
// test_pwlf_itou
// ****************************************************************************
START_TEST(test_pwlf_itou)
{
  ck_assert_uint_eq(pwlf_itou(16000), 48768);
  ck_assert_uint_eq(pwlf_itou(-742), 32026);
}
END_TEST


// ****************************************************************************
// test_pwlf_init
// ****************************************************************************
START_TEST(test_pwlf_init)
{
  static pwlf f = PWLF_INIT(8);
  ck_assert_uint_eq(pwlf_get_size(&f), 8);
  ck_assert_uint_eq(pwlf_get_count(&f), 0);
}
END_TEST

// ****************************************************************************
// test_pwlf_2point_identity
// ****************************************************************************
START_TEST(test_pwlf_2point_identity)
{
  static pwlf f = PWLF_INIT(8);
  pwlf_add_node(&f, 0, 0);
  pwlf_add_node(&f, UINT16_MAX, UINT16_MAX);
  ck_assert_uint_eq(pwlf_get_size(&f), 8);
  ck_assert_uint_eq(pwlf_get_count(&f), 2);

  uint16_t i;
  for (i = 0; i < UINT16_MAX - 16; i += 16) {
    ck_assert_uint_eq(pwlf_value(&f, i), i);
  }
}
END_TEST


// ****************************************************************************
// test_pwlf_2point
// ****************************************************************************
START_TEST(test_pwlf_2point)
{
  static pwlf f = PWLF_INIT(8);
  pwlf_add_node(&f, 0, 0);
  pwlf_add_node(&f, UINT16_MAX, 15000);
  ck_assert_uint_eq(pwlf_get_size(&f), 8);
  ck_assert_uint_eq(pwlf_get_count(&f), 2);

  ck_assert_uint_eq(pwlf_value(&f, 0), 0);
  ck_assert_uint_eq(pwlf_value(&f, UINT16_MAX/2 + 1), 15000/2);
  ck_assert_uint_eq(pwlf_value(&f, UINT16_MAX), 15000);
}
END_TEST


// ****************************************************************************
// test_pwlf_2point_decreasing
// ****************************************************************************
START_TEST(test_pwlf_2point_decreasing)
{
  static pwlf f = PWLF_INIT(8);
  pwlf_add_node(&f, 0, UINT16_MAX);
  pwlf_add_node(&f, UINT16_MAX, 0);
  ck_assert_uint_eq(pwlf_get_size(&f), 8);
  ck_assert_uint_eq(pwlf_get_count(&f), 2);

  uint16_t i;
  for (i = 0; i < UINT16_MAX - 16; i += 16) {
    ck_assert(pwlf_value(&f, i) <= UINT16_MAX - i + 1);
    ck_assert(pwlf_value(&f, i) >= UINT16_MAX - i - 1);
  }
}
END_TEST


// ****************************************************************************
// test_pwlf_2point_signed
// ****************************************************************************
START_TEST(test_pwlf_2point_signed)
{
  static pwlf f = PWLF_INIT(8);
  pwlf_add_node(&f, 0, pwlf_itou(16000));
  pwlf_add_node(&f, UINT16_MAX, pwlf_itou(-742));
  ck_assert_uint_eq(pwlf_get_size(&f), 8);
  ck_assert_uint_eq(pwlf_get_count(&f), 2);


  ck_assert_int_eq(pwlf_utoi(pwlf_value(&f, 0)), 16000);
  ck_assert_int_eq(pwlf_utoi(pwlf_value(&f, UINT16_MAX)), -742);

  uint16_t i;
  for (i = 0; i < UINT16_MAX - 16; i += 16) {
    int16_t value = pwlf_utoi(pwlf_value(&f, i));
    int16_t expected_value = UROUND(expected(i, 0, UINT16_MAX, 16000, -742));
    ck_assert(value >= expected_value - 1);
    ck_assert(value <= expected_value + 1);
    printf("i: %u, v: %d\n", i, value);
  }
}
END_TEST

// ****************************************************************************
//                           Test suite setup
// ****************************************************************************
Suite *pwlf_suite(void)
{
  Suite *s = suite_create("PWLF");

  TCase *tc_pwlf_itou = tcase_create("itou");
  tcase_add_checked_fixture(tc_pwlf_itou, setup, teardown);
  tcase_add_test(tc_pwlf_itou, test_pwlf_itou);
  suite_add_tcase(s, tc_pwlf_itou);

  TCase *tc_pwlf_init = tcase_create("Init");
  tcase_add_checked_fixture(tc_pwlf_init, setup, teardown);
  tcase_add_test(tc_pwlf_init, test_pwlf_init);
  suite_add_tcase(s, tc_pwlf_init);

  TCase *tc_pwlf_2point_identity = tcase_create("Two-point identity");
  tcase_add_checked_fixture(tc_pwlf_2point_identity, setup, teardown);
  tcase_add_test(tc_pwlf_2point_identity, test_pwlf_2point_identity);
  suite_add_tcase(s, tc_pwlf_2point_identity);

  TCase *tc_pwlf_2point = tcase_create("Two-point");
  tcase_add_checked_fixture(tc_pwlf_2point, setup, teardown);
  tcase_add_test(tc_pwlf_2point, test_pwlf_2point);
  suite_add_tcase(s, tc_pwlf_2point);

  TCase *tc_pwlf_2point_decreasing = tcase_create("Two-point decreasing");
  tcase_add_checked_fixture(tc_pwlf_2point_decreasing, setup, teardown);
  tcase_add_test(tc_pwlf_2point_decreasing, test_pwlf_2point_decreasing);
  suite_add_tcase(s, tc_pwlf_2point_decreasing);

  TCase *tc_pwlf_2point_signed = tcase_create("Two-point signed");
  tcase_add_checked_fixture(tc_pwlf_2point_signed, setup, teardown);
  tcase_add_test(tc_pwlf_2point_signed, test_pwlf_2point_signed);
  suite_add_tcase(s, tc_pwlf_2point_signed);

  return s;
}
