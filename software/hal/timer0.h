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

// Operations
#define TIMER0_INIT

// Output channels
#define TIMER0_OCA_DISCONNECT   TCCR0A &= ~(_BV(COM0A1) | _BV(COM0A0))
#define TIMER0_OCB_DISCONNECT   TCCR0A &= ~(_BV(COM0B1) | _BV(COM0B0))

// Interrups
#define TIMER0_OCA_INT_ENABLE   TIMSK0 |= OCIE0A
#define TIMER0_OCB_INT_ENABLE   TIMSK0 |= OCIE0B
#define TIMER0_OVF_INT_ENABLE   TIMSK0 |= TOIE0

// Modes
#define TIMER0_SET_MODE_NORMAL					\
  do {								\
    TCCR0B &= ~(_BV(WGM02));					\
    TCCR0A &= ~(_BV(WGM01) | _BV(WGM00));			\
  } while (0)
#define TIMER0_SET_MODE_CTC_OCRA				\
  do {								\
    TCCR0B &= ~(_BV(WGM02));					\
    TCCR0A |= _BV(WGM01);					\
    TCCR0A &= ~(_BV(WGM00));					\
  } while (0)
#define TIMER0_SET_MODE_FAST_PWM_0FF				\
  do {								\
    TCCR0B &= ~(_BV(WGM02));					\
    TCCR0A |= (_BV(WGM01) | _BV(WGM00));			\
  } while (0)
#define TIMER0_SET_MODE_FAST_PWM_OCRA				\
  do {								\
    TCCR0B |= _BV(WGM02);					\
    TCCR0A |= (_BV(WGM01) | _BV(WGM00));			\
  } while (0)
#define TIMER0_SET_MODE_PWM_PHASE_CORRECT_0FF			\
  do {								\
    TCCR0B &= ~(_BV(WGM02));					\
    TCCR0B &= ~(_BV(WGM01));					\
    TCCR0A |= _BV(WGM00);					\
  } while (0)
#define TIMER0_SET_MODE_PWM_PHASE_CORRECT_OCRA			\
  do {								\
    TCCR0B |= _BV(WGM02);					\
    TCCR0B &= ~(_BV(WGM01));					\
    TCCR0A |= _BV(WGM00);					\
  } while (0)


// Clock sources
#define TIMER0_SET_CLOCK_SRC_DISABLED			\
  TCCR0B ~= ~(_BV(CS02) | _BV(CS01) | _BV(CS00))
#define TIMER0_SET_CLOCK_SRC_FULL_SPEED		\
  do {						\
    TCCR0B ~= ~(_BV(CS02) | _BV(CS01));		\
    TCCR0B |= _BV(CS00);			\
  } while (0)
#define TIMER0_SET_CLOCK_SRC_PRESCALE_8		\
  do {						\
    TCCR0B ~= ~(_BV(CS02) | _BV(CS00));		\
    TCCR0B |= _BV(CS01);			\
  }
#define TIMER0_SET_CLOCK_SRC_PRESCALE_64	\
  do {						\
    TCCR0B ~= ~(_BV(CS02));			\
    TCCR0B |= (_BV(CS01) | _BV(CS00));		\
  } while (0)
#define TIMER0_SET_CLOCK_SRC_PRESCALE_256	\
  do {						\
    TCCR0B |= _BV(CS02);			\
    TCCR0B ~= ~(_BV(CS01) | _BV(CS00));		\
  } while (0)
#define TIMER0_SET_CLOCK_SRC_PRESCALE_1024	\
  do {						\
    TCCR0B |= (_BV(CS02) | _BV(CS00));		\
    TCCR0B ~= ~(_BV(CS01));			\
  } while (0)
#define TIMER0_SET_CLOCK_SRC_EXT_FALLING	\
  do {						\
    TCCR0B |= (_BV(CS02) | _BV(CS01));		\
    TCCR0B ~= ~(_BV(CS00));			\
  } while (0)
#define TIMER0_SET_CLOCK_SRC_EXT_RISING		\
  TCCR0B |= (_BV(CS02) | _BV(CS01) | _BV(CS00))

#define TIMER0_SET_CNTR(val) TCNT0 = val
#define TIMER0_GET_CNTR      TCNT0

// Constants
#define TIMER0_SIZE  8

// Interrupt vectors
#define TIMER0_OCA_VECT  TIMER0_COMPA_vect
#define TIMER0_OCB_VECT  TIMER0_COMPB_vect
#define TIMER0_OVF_VECT  TIMER0_OVF_vect


#endif
