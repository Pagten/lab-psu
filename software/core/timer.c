/*
 * timer.c
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
 * @file timer.c
 * @author Pieter Agten <pieter.agten@gmail.com>
 * @date 14 Jan 2014
 */

#include "timer.h"

void timer_set(timer* t, clock_time_t delay)
{
  t->start = clock_get_time();
  t->delay = delay;
}

void timer_reset(timer* t)
{
  t->start += t->delay;
}

void timer_restart(timer* t)
{
  t->start = clock_get_time();
}

bool timer_expired_at(timer* t, clock_time_t time)
{
  return (time - t->start) >= t->delay;
}

bool timer_expired(timer* t)
{
  return timer_expired_at(t, clock_get_time());
}

clock_time_t timer_remaining_at(timer* t, clock_time_t time)
{
  return timer_expired_at(t, time) ? 0 : (t->start + t->delay - time);
}

clock_time_t timer_remaining(timer* t)
{
  return timer_remaining_at(t, clock_get_time());
}

