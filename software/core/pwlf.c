/*
 * pwlf.c
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
 * @file pwlf.c
 * @author Pieter Agten (pieter.agten@gmail.com)
 * @date 10 Oct 2014
 */

#include "pwlf.h"

#include <stdint.h>

#include "util/math.h"

void pwlf_clear(pwlf* f)
{
  f->count = 0;
}


inline
uint8_t pwlf_get_count(pwlf* f)
{
  return f->count;
}


inline
uint8_t pwlf_get_size(pwlf* f)
{
  return f->max_count;
}


uint16_t pwlf_get_x(pwlf* f, uint8_t i)
{
  if (i >= f->count) {
    return 0;
  }

  return f->values[i].x;
}


uint16_t pwlf_get_y(pwlf* f, uint8_t i)
{
  if (i >= f->count) {
    return 0;
  }

  return f->values[i].y;
}

pwlf_add_node_status
pwlf_add_node(pwlf* f, uint16_t x, uint16_t y)
{
  if (f->count == f->max_count) {
    return PWLF_ADD_NODE_FULL;
  }
  if (f->count > 0 && x <= f->values[f->count].x) {
    return PWLF_ADD_NODE_INVALID_X;
  }

  f->values[f->count].x = x;
  f->values[f->count].y = y;
  f->count += 1;
  return PWLF_ADD_NODE_OK;
}


pwlf_remove_node_status
pwlf_remove_node(pwlf* f)
{
  if (f->count == 0) {
    return PWLF_REMOVE_NODE_EMPTY;
  }

  f->count -= 1;
  return PWLF_REMOVE_NODE_OK;
}


// p0.x must be <= p1.x
static inline
uint16_t polate(uint16_t x, pwlf_pair p0, pwlf_pair p1)
{
  float dx = ((float)(p1.y - p0.y)) / (p1.x - p0.x);
  uint16_t result = p0.y + UROUND(dx * (x - p0.x));

  // Detect overflow
  if (p0.y <= p1.y) {
    // Increasing function
    if (x < p0.x && p0.y < result) {
      return 0;
    }
    if (p1.x < x && result < p1.y) {
      return UINT16_MAX;
    }
  } else {
    // Decreasing function
    if (x < p0.x && result < p0.y) {
      return UINT16_MAX;
    }
    if (p1.x < x && p1.y < result) {
      return 0;
    }
  }
  return result;
}

uint16_t pwlf_value(pwlf* f, uint16_t x)
{
  if (f->count <= 1) {
    // Not enough nodes defined to interpolate or extrapolate
    return 0;
  }

  // Find node with smallest x value that is >= x
  uint8_t i = 0;
  while (i < f->count && f->values[i].x < x) {
    i += 1;
  }

  uint8_t i0, i1;
  if (i == 0) {
    // x is smaller than or equal to the first node, extrapolate
    i0 = i;
    i1 = i + 1;
  } else {
    // x is between i-1 and i (interpolate) or larger than i (extrapolate)
    i0 = i - 1;
    i1 = i;
  }
  return polate(x, f->values[i0], f->values[i1]);
}


inline
uint16_t pwlf_itou(int16_t v)
{
  return v + (UINT16_MAX/2 + 1);
}

inline
int16_t pwlf_utoi(uint16_t v)
{
  return v - (UINT16_MAX/2 + 1);
}
