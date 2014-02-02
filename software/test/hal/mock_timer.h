/*
 * mock_timer.h
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

#ifndef MOCK_TIMER_H
#define MOCK_TIMER_H

#include <stdbool.h>
#include <stdint.h>
#include "util/pp_magic.h"


typedef enum {
  CH_OCA,
  CH_OCB,
} tmr_channel;

typedef enum {
  INTR_OCA,
  INTR_OCB,
  INTR_OVF,
} tmr_interrupt;

typedef enum {
  M_NORMAL,
  M_CTC_OCRA,
  M_FAST_PWM_0FF,
  M_FAST_PWM_OCRA,
  M_PHASE_CORRECT_0FF,
  M_PHASE_CORRECT_OCRA,
} tmr_mode;

typedef enum {
  CS_DISABLED,
  CS_FULL_SPEED,
  CS_PRESCALE_8,
  CS_PRESCALE_64,
  CS_PRESCALE_256,
  CS_PRESCALE_1024,
  CS_EXT_FALLING,
  CS_EXT_RISING,
} tmr_clock_src;

typedef struct {
  unsigned char nb_bits;
  uint16_t cntr;
  uint16_t ocra;
  uint16_t ocrb;
  tmr_mode mode;
  bool clock_enabled;
  bool oca_intr_enabled;
  bool ocb_intr_enabled;
  bool ovf_intr_enabled;
  void (*oca_vect)(void);
  void (*ocb_vect)(void);
  void (*ovf_vect)(void);
} mock_timer;


mock_timer mock_timer0_create();
mock_timer mock_timer1_create();
mock_timer mock_timer2_create();
void mock_timer_init(mock_timer* tmr);
void mock_timer_tick(mock_timer* tmr);
void mock_timer_channel_disconnect(mock_timer* tmr, tmr_channel ch);
void mock_timer_set_intr_enabled(mock_timer* tmr, tmr_interrupt intr, bool val);
void mock_timer_set_mode(mock_timer* tmr, tmr_mode mode);
void mock_timer_set_clock(mock_timer* tmr, tmr_clock_src cs); 
void mock_timer_set_cntr(mock_timer* tmr, uint16_t val);
uint8_t mock_timer_get_cntr8(mock_timer* tmr);
uint16_t mock_timer_get_cntr16(mock_timer* tmr);


#define MOCK_TIMER_TICK(tmr)  CAT(tmr,_TICK)


#endif
