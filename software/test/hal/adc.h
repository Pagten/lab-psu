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

#include "util/bit.h"
#include "util/pp_magic.h"

// Available voltage references:
// 1) AREF
// 2) AVCC
// 3) INTERNAL

// TODO: implement
#define ADC_SET_VREF_AREF			
#define ADC_SET_VREF_AVCC			
#define ADC_SET_VREF_INTERNAL			
#define ADC_SET_VREF(ref) CAT(ADC_SET_VREF_,ref)


// ADC conversion result alignment
// 1) LEFT
// 2) RIGHT
#define ADC_SET_ADJUST_LEFT			
#define ADC_SET_ADJUST_RIGHT			
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

static inline
void ADC_SET_CHANNEL(adc_channel ch)
{
}


static inline
void ADC_DIGITAL_INPUT_ENABLE(adc_channel ch)
{
}

static inline
void ADC_DIGITAL_INPUT_DISABLE(adc_channel ch)
{

}


#define ADC_ENABLE()				
#define ADC_DISABLE()				

#define ADC_START_CONVERSION()			
#define ADC_IS_BUSY() (false)

#define ADC_AUTO_TRIGGER_ENABLE()		
#define ADC_AUTO_TRIGGER_DISABLE()		

#define ADC_INTERRUPT_ENABLE() 
#define ADC_INTERRUPT_DISABLE()



#define ADC_SET_PRESCALER_DIV(num) 



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

static inline
void ADC_SET_AUTO_TRIGGER_SRC(adc_trigger_src src)
{
}


#define ADC_GET_VALUE() 0

#define IS_ADC_INTERRUPT_FLAG_SET()       (false)

// Transfer complete interrupt
#define ADC_CONVERSION_COMPLETE_VECT  void adc_conversion_complete_vect(void)

#define ADC_CC_INTERRUPT_ENABLE()   
#define ADC_CC_INTERRUPT_DISABLE()  


#endif
