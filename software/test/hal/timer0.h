/*
 * timer0.h
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

#ifndef TIMER0_H
#define TIMER0_H

#include "mock_timer.h"
#include "util/pp_magic.h"

#define MOCK_TMR _mock_timer0
extern mock_timer MOCK_TMR;

// Operations
#define TIMER0_INIT  mock_timer_init(&MOCK_TMR)

// Output channels
#define TIMER0_OCA_DISCONNECT  mock_timer_channel_disconnect(&MOCK_TMR, CH_OCA)
#define TIMER0_OCB_DISCONNECT  mock_timer_channel_disconnect(&MOCK_TMR, CH_OCB)

// Interrupts
#define TIMER0_OCA_INTR_ENABLE \
  mock_timer_set_intr_enabled(&MOCK_TMR, INTR_OCA, true)
#define TIMER0_OCB_INTR_ENABLE \
  mock_timer_set_intr_enabled(&MOCK_TMR, INTR_OCB, true)
#define TIMER0_OVF_INTR_ENABLE \
  mock_timer_set_intr_enabled(&MOCK_TMR, INTR_OVF, true)

// Modes
#define TIMER0_SET_MODE_NORMAL			\
  mock_timer_set_mode(&MOCK_TMR, M_NORMAL)
#define TIMER0_SET_MODE_CTC_OCRA		\
  mock_timer_set_mode(&MOCK_TMR, M_CTC_OCRA)
#define TIMER0_SET_MODE_FAST_PWM_0FF		\
  mock_timer_set_mode(&MOCK_TMR, M_FAST_PWM_0FF)
#define TIMER0_SET_MODE_FAST_PWM_OCRA		\
  mock_timer_set_mode(&MOCK_TMR, M_FAST_PWM_OCRA)
#define TIMER0_SET_MODE_PWM_PHASE_CORRECT_0FF	\
  mock_timer_set_mode(&MOCK_TMR, M_PHASE_CORRECT_0FF)
#define TIMER0_SET_MODE_PWM_PHASE_CORRECT_OCRA  \
  mock_timer_set_mode(&MOCK_TMR, M_PHASE_CORRECT_OCRA)


// Clock sources
#define TIMER0_SET_CLOCK_DISABLED		\
  mock_timer_set_clock(&MOCK_TMR, CS_DISABLED)
#define TIMER0_SET_CLOCK_FULL_SPEED	        \
  mock_timer_set_clock(&MOCK_TMR, CS_FULL_SPEED)
#define TIMER0_SET_CLOCK_PRESCALE_8		\
  mock_timer_set_clock(&MOCK_TMR, CS_PRESCALE_8)
#define TIMER0_SET_CLOCK_PRESCALE_64	\
  mock_timer_set_clock(&MOCK_TMR, CS_PRESCALE_64)
#define TIMER0_SET_CLOCK_PRESCALE_256	\
  mock_timer_set_clock(&MOCK_TMR, CS_PRESCALE_256)
#define TIMER0_SET_CLOCK_PRESCALE_1024	\
  mock_timer_set_clock(&MOCK_TMR, CS_PRESCALE_1024)
#define TIMER0_SET_CLOCK_EXT_FALLING	\
  mock_timer_set_clock(&MOCK_TMR, CS_EXT_FALLING)
#define TIMER0_SET_CLOCK_EXT_RISING		\
  mock_timer_set_clock(&MOCK_TMR, CS_EXT_RISING)

#define TIMER0_OCA_SET_OCR(val)  mock_timer_set_ocr8(&MOCK_TMR, CH_OCA, val)
#define TIMER0_OCA_GET_OCR       mock_timer_get_ocr8(&MOCK_TMR, CH_OCA)

#define TIMER0_OCB_SET_OCR(val)  mock_timer_set_ocr8(&MOCK_TMR, CH_OCB, val)
#define TIMER0_OCB_GET_OCR       mock_timer_get_ocr8(&MOCK_TMR, CH_OCB)

#define TIMER0_SET_CNTR(val)  mock_timer_set_cntr8(&MOCK_TMR, val)
#define TIMER0_GET_CNTR       mock_timer_get_cntr8(&MOCK_TMR)

// Constants
#define TIMER0_SIZE  8

// Interrupt vectors (cannot use MOCK_TMR here)
#define TIMER0_OCA_VECT  void _mock_timer0_oca_vect(void)
#define TIMER0_OCB_VECT  void _mock_timer0_ocb_vect(void)
#define TIMER0_OVF_VECT  void _mock_timer0_ovf_vect(void)

// Mock timer operations
#define TIMER0_TICK mock_timer_tick(&MOCK_TMR)

#endif
