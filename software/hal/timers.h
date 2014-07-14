/*
 * timers.h
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

#ifndef TIMERS_H
#define TIMERS_H

// Timers
// 1) timer0
// 2) timer1
// 3) timer2

// Channels
// 1) OCA
// 2) OCB

// Timer modes
// 1) NORMAL
// 2) CTC_OCRA
// 3) CTC_ICR*
// 4) FAST_PWM_0FF
// 5) FAST_PWM_1FF*
// 6) FAST_PWM_3FF*
// 7) FAST_PWM_ICR*
// 8) FAST_PWM_OCRA
// 9) PWM_PHASE_CORRECT_0FF
// 10) PWM_PHASE_CORRECT_1FF*
// 11) PWM_PHASE_CORRECT_3FF*
// 12) PWM_PHASE_CORRECT_ICR*
// 13) PWM_PHASE_CORRECT_OCRA
// 14) PWM_PHASE_AND_FREQ_CORRECT_ICR*
// 15) PWM_PHASE_AND_FREQ_CORRECT_OCRA*
// *: 16-bit timers only

// Clock sources
// 1) DISABLED
// 2) FULL_SPEED
// 3) PRESCALE_8
// 4) PRESCALE_32
// 5) PRESCALE_64
// 6) PRESCALE_128
// 7) PRESCALE_256
// 8) PRESCALE_1024
// 9) EXT_FALLING
// 10) EXT_RISING
// (not all clock sources available on all timers)

// Interrupts
// 1) OCA
// 2) OCB
// 3) OVF

#include "util/pp_magic.h"
#include "hal/timer0.h"
//#include "hal/timer1.h"
//#include "hal/timer2.h"

// Operations
#define TMR_INIT(tmr)                   CAT(tmr,_INIT)
#define TMR_CHANNEL_DISCONNECT(tmr,ch)  CAT(tmr,_,ch,_DISCONNECT)
#define TMR_INTERRUPT_ENABLE(tmr,intr)  CAT(tmr,_,intr,_INTR_ENABLE)
#define TMR_SET_MODE(tmr,mode)          CAT(tmr,_SET_MODE_,mode)
#define TMR_SET_PRESCALER(tmr,val)      TMR_SET_CLOCK(tmr, CAT(PRESCALE_,val))
#define TMR_SET_CLOCK(tmr,val)          CAT(tmr,_SET_CLOCK_,val)
#define TMR_SET_OCR(tmr,ch,val)         CAT(tmr,_,ch,_SET_OCR(val))
#define TMR_GET_OCR(tmr,ch)             CAT(tmr,_,ch,_GET_OCR)
#define TMR_SET_CNTR(tmr,val)           CAT(tmr,_SET_CNTR(val))
#define TMR_GET_CNTR(tmr)               CAT(tmr,_GET_CNTR)

#define TMR_IS_INTERRUPT_FLAG_SET(tmr,intr)	\
  CAT(tmr,_IS_,intr,_INTERRUPT_FLAG_SET)

// Constants
#define TMR_SIZE(tmr)                   CAT(tmr,_SIZE)

// Interrupt vectors
#define TMR_INTERRUPT_VECT(tmr,intr)    CAT(tmr,_,intr,_VECT)


#endif
