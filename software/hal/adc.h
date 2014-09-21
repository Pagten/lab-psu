/*
 * adc.h
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

#ifndef HAL_ADC_H
#define HAL_ADC_H

#include "util/pp_magic.h"

// Available voltage references:
// 1) AREF
// 2) AVCC
// 3) INTERNAL

#define ADC_SET_VREF_AREF			\
  do {						\
    ADMUX &= ~(_BV(REFS1) | _BV(REFS0));	\
  } while(false)
#define ADC_SET_VREF_AVCC			\
  do {						\
    ADMUX &= ~_BV(REFS1);			\
    ADMUX |= _BV(REFS0);			\
  } while(false)
#define ADC_SET_VREF_INTERNAL			\
  do {						\
    ADMUX |= _BV(REFS1) | _BV(REFS0);		\
  } while(false)
#define ADC_SET_VREF(ref) CAT(ADC_SET_VREF_,ref)


// ADC conversion result alignment
// 1) LEFT
// 2) RIGHT
#define ADC_SET_ADJUST_LEFT			\
  do {						\
    ADMUX |= _BV(ADLAR);			\
  } while(false)
#define ADC_SET_ADJUST_RIGHT			\
  do {						\
      ADMUX &= ~_BV(ADLAR);			\
  } while(false)
#define ADC_SET_ADJUST(dir) CAT(ADC_SET_ADJUST_,dir)

typedef enum {
  ADC_CHANNEL_0 = 0,
  ADC_CHANNEL_1,
  ADC_CHANNEL_2,
  ADC_CHANNEL_3,
  ADC_CHANNEL_4,
  ADC_CHANNEL_5,
  ADC_CHANNEL_6,
  ADC_CHANNEL_7,
  ADC_CHANNEL_8,
  ADC_CHANNEL_INTERNAL_VREF = 14,
  ADC_CHANNEL_GND = 15
} adc_channel;

inline
void ADC_SET_CHANNEL(adc_channel ch)
{
  if (ch <= 8 || ch == 14 || ch == 15) {
    ADMUX &= 0xF0;
    ADMUX |= ch;
  }
}


#define ADC_ENABLE()				\
  do {						\
    ADCSRA |= _BV(ADEN);			\
  } while(false)
#define ADC_DISABLE()				\
  do {						\
    ADCSRA &= ~_BV(ADEN);			\
  } while(false)

#define ADC_START_CONVERSION()			\
  do {						\
    ADCSRA |= _BV(ADSC);			\
  } while(false)

#define ADC_IS_BUSY() (ADCSRA & _BV(ADSC))

#define ADC_AUTO_TRIGGER_ENABLE()		\
  do {						\
    ADCSRA |= _BV(ADATA);			\
  } while(false)
#define ADC_AUTO_TRIGGER_DISABLE()		\
  do {						\
    ADCSRA &= ~_BV(ADATA);			\
  } while(false)


typedef enum {
  ADC_TRIGGER_FREERUNNING = 0,
  ADC_TRIGGER_ANALOG_COMPARATOR,
  ADC_TRIGGER_EXTERNAL_INTERRUPT0,
  ADC_TRIGGER_TIMER0_CHANNEL_A,
  ADC_TRIGGER_TIMER0_OVERFLOW,
  ADC_TRIGGER_TIMER1_CHANNEL_B,
  ADC_TRIGGER_TIMER1_OVERFLOW,
  ADC_TRIGGER_TIMER1_CAPTURE_EVENT,
} adc_trigger_src;

inline
void ADC_SET_AUTO_TRIGGER_SRC(adc_trigger_src src)
{
  ADCSRB &= 0xF8;
  ADCSRB |= (src & 0x07);
}


#endif
