/*
 * mock_timers.c
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

#include "mock_timer.h"

#include <stdio.h>
#include <stdlib.h>


// Mock timer 0
void _mock_timer0_oca_vect(void) __attribute__((weak));
void _mock_timer0_oca_vect(void)
{
  fputs("error: call to undefined _mock_timer0_oca_vect", stderr);
  abort();
}
void _mock_timer0_ocb_vect(void) __attribute__((weak));
void _mock_timer0_ocb_vect(void)
{
  fputs("error: call to undefined _mock_timer0_ocb_vect", stderr);
  abort();
}
void _mock_timer0_ovf_vect(void) __attribute__((weak));
void _mock_timer0_ovf_vect(void)
{
  fputs("error: call to undefined _mock_timer0_ovf_vect", stderr);
  abort();
}

mock_timer _mock_timer0 = {
  .nb_bits = 8, 
  .oca_vect = _mock_timer0_oca_vect,
  .ocb_vect = _mock_timer0_ocb_vect,
  .ovf_vect = _mock_timer0_ovf_vect,
 };


// Mock timer 1
void _mock_timer1_oca_vect(void) __attribute__((weak));
void _mock_timer1_oca_vect(void)
{
  fputs("error: call to undefined _mock_timer1_oca_vect", stderr);
  abort();
}
void _mock_timer1_ocb_vect(void) __attribute__((weak));
void _mock_timer1_ocb_vect(void)
{
  fputs("error: call to undefined _mock_timer1_ocb_vect", stderr);
  abort();
}
void _mock_timer1_ovf_vect(void) __attribute__((weak));
void _mock_timer1_ovf_vect(void)
{
  fputs("error: call to undefined _mock_timer1_ovf_vect", stderr);
  abort();
}

mock_timer _mock_timer1 = {
  .nb_bits = 16, 
  .oca_vect = _mock_timer1_oca_vect,
  .ocb_vect = _mock_timer1_ocb_vect,
  .ovf_vect = _mock_timer1_ovf_vect,
 };


// Mock timer 2
void _mock_timer2_oca_vect(void) __attribute__((weak));
void _mock_timer2_oca_vect(void)
{
  fputs("error: call to undefined _mock_timer2_oca_vect", stderr);
  abort();
}
void _mock_timer2_ocb_vect(void) __attribute__((weak));
void _mock_timer2_ocb_vect(void)
{
  fputs("error: call to undefined _mock_timer2_ocb_vect", stderr);
  abort();
}
void _mock_timer2_ovf_vect(void) __attribute__((weak));
void _mock_timer2_ovf_vect(void)
{
  fputs("error: call to undefined _mock_timer2_ovf_vect", stderr);
  abort();
}

mock_timer _mock_timer2 = {
  .nb_bits = 8, 
  .oca_vect = _mock_timer2_oca_vect,
  .ocb_vect = _mock_timer2_ocb_vect,
  .ovf_vect = _mock_timer2_ovf_vect,
};
