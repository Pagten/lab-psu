/*
 * adc.c
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
 * @file adc.c
 * @author Pieter Agten <pieter.agten@gmail.com>
 * @date 21 Sep 2014
 *
 * The ADC module provides an API to read out the MCU's analog to digital
 * converters.
 */

#include "adc.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <util/atomic.h>

#include "core/process.h"
#include "hal/adc.h"
#include "hal/interrupt.h"
#include "util/debug.h"

PROCESS(adc_process);

#define FLAG_ADC_READY 0x10

#define EVENT_ADC_LIST_CHANGED         (process_event_t)0x00
#define EVENT_ADC_CONVERSION_COMPLETE  (process_event_t)0x01

static adc* adcs;
static adc* volatile next_next_adc;
static adc* next_adc_to_consider;

void init_adc(void)
{
  adcs = NULL;
  next_next_adc = NULL;
  next_adc_to_consider = NULL;

  ADC_SET_VREF(AREF);
  ADC_SET_ADJUST(RIGHT);
  ADC_SET_AUTO_TRIGGER_SRC(ADC_TRIGGER_FREERUNNING);
  ADC_AUTO_TRIGGER_ENABLE();
  ADC_SET_PRESCALER_DIV(128);
  ADC_SET_CHANNEL(ADC_CHANNEL_GND);
  ADC_CC_INTERRUPT_ENABLE();
  ADC_ENABLE();
  ADC_START_CONVERSION();

  process_start(&adc_process);
}

static bool
is_valid_adc_channel(adc_channel channel)
{
  return 0 <= channel && channel <= 7; 
}

static bool
is_valid_resolution(adc_resolution resolution)
{
  return resolution <= ADC_RESOLUTION_16BIT;
}

static bool
is_valid_skip(adc_skip skip)
{
  return skip == ADC_SKIP_0
    || skip == ADC_SKIP_1
    || skip == ADC_SKIP_3
    || skip == ADC_SKIP_7
    || skip == ADC_SKIP_15;
}

static
bool adc_in_list(adc* adc0)
{
  adc** a = &adcs;
  while (*a != NULL) {
    if (*a == adc0) {
      return true;
    }
    a = &((*a)->next);
  }

  return false;
}


adc_init_status
adc_init(adc* adc, adc_channel channel, adc_resolution resolution,
	 adc_skip skip, process* process)
{
  if (adc_in_list(adc)) {
    return ADC_INIT_ALREADY_IN_LIST;
  }
  if (! is_valid_adc_channel(channel)) {
    return ADC_INIT_INVALID_CHANNEL;
  }
  if (! is_valid_resolution(resolution)) {
    return ADC_INIT_INVALID_RESOLUTION;
  }
  if (! is_valid_skip(skip)) {
    return ADC_INIT_INVALID_SKIP;
  }

  adc->value = 0;
  adc->flags_channel = channel;
  adc->resolution = resolution;
  adc->skip = skip;
  adc->process = process;
  return ADC_INIT_OK;
}

static inline
bool adc_is_ready(adc* adc)
{
  return adc->flags_channel & FLAG_ADC_READY;
}

static inline
void adc_set_ready(adc* adc, bool ready)
{
  if (ready) {
    adc->flags_channel |= FLAG_ADC_READY;
  } else {
    adc->flags_channel &= ~FLAG_ADC_READY;
  }
}

static inline
void set_samples_remaining(adc* adc)
{
  adc->samples_remaining = 1 << (2 * adc->resolution);
}

adc_channel adc_get_channel(adc* adc)
{
  return adc->flags_channel & 0x0F;
}

bool adc_enable(adc* adc0)
{
  // Find position in ADC list
  adc** a = &adcs;
  while (*a != NULL && adc_get_channel(*a) <= adc_get_channel(adc0)) {
    if (*a == adc0) {
      return false;
    }
    a = &((*a)->next);
  }

  // Initialize ADC
  adc0->next_value = 0;
  set_samples_remaining(adc0);

  // Add new ADC to list
  adc0->next = *a;
  *a = adc0;
  adc_set_ready(adc0, true);

  // Disable digital input on channel to save power
  ADC_DIGITAL_INPUT_DISABLE(adc_get_channel(adc0));

  // Notify the ADC process that the ADC list has changed
  process_post_event(&adc_process, EVENT_ADC_LIST_CHANGED, PROCESS_DATA_NULL);

  return true;
}

bool adc_disable(adc* adc0)
{
  bool only_adc_for_channel = true;

  // Find position in list
  adc** a = &adcs;
  while (*a != NULL && *a != adc0) {
    if (adc_get_channel(*a) == adc_get_channel(adc0)) {
      only_adc_for_channel = false;
    }
    a = &((*a)->next);
  }

  if (*a == NULL) {
    return false;
  }

  // Make sure we do not consider this ADC for the next channel to queue.
  if (next_adc_to_consider == adc0) {
    next_adc_to_consider = adc0->next;
  }

  // Remove ADC from list
  *a = (*a)->next;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    adc_set_ready(adc0, false);
  }
  adc0->next = NULL;

  // Check channel of next ADC in list
  only_adc_for_channel = only_adc_for_channel &&
    ((*a) == NULL || adc_get_channel(*a) != adc_get_channel(adc0));

  // Re-enable the digital input if there are no other ADC's for the same
  // channel in the list
  if (only_adc_for_channel) {
    ADC_DIGITAL_INPUT_ENABLE(adc_get_channel(adc0));
  }
  return true;
}

uint16_t adc_get_value(adc* adc)
{
  return adc->value;
}

static inline bool
should_skip(adc* adc, uint8_t period)
{
  return (adc->skip & period) != 0;
}

static inline void
set_value(adc* adc)
{
  if (adc->resolution <= ADC_RESOLUTION_13BIT) {
    uint8_t shift = 6 - (2 * adc->resolution);
    adc->value = adc->next_value << shift;
  } else {
    uint8_t shift = (2 * adc->resolution) - 6;
    adc->value = adc->next_value >> (__uint24)shift;
  }
}

static inline void
handle_completed_conversion(adc* adc0)
{
  if (adc0 != NULL && adc0->samples_remaining == 0) {
    //    SET_DEBUG_LED(0);

    // We have enough samples for a full measurement
    set_value(adc0);
    adc0->next_value = 0;
    set_samples_remaining(adc0);
    if (adc0->process != NULL) {
      process_post_event(adc0->process, ADC_MEASUREMENT_COMPLETED,
			 (process_data_t)adc0);
    }
    //    CLR_DEBUG_LED(0);
  }
}

static inline void
queue_next_next_adc()
{
  static uint8_t period = 0;
  adc* adc = next_adc_to_consider;
  while(next_next_adc == NULL && adcs != NULL) {
    while(adc != NULL && should_skip(adc, period)) {
      adc = adc->next;
    }
    
    if (adc != NULL) {
      // Found next next adc, let's append it to the queue
      adc_set_ready(adc, true);
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
	next_next_adc = adc;
      }
      next_adc_to_consider = adc->next;
    } else {
      // Finished current period
      adc = adcs;
      period += 1;
    }
  }
}

PROCESS_THREAD(adc_process)
{
  PROCESS_BEGIN();

  while(true) {
    PROCESS_WAIT_EVENT_UNTIL(ev == EVENT_ADC_CONVERSION_COMPLETE || 
			     ev == EVENT_ADC_LIST_CHANGED);

    if (ev == EVENT_ADC_CONVERSION_COMPLETE) {
      handle_completed_conversion((adc*)data);
    }
    queue_next_next_adc();
  }

  PROCESS_END();
}

//TODO: isr queue is never full when there are less than 3 ADCs in the
//adc queue!
INTERRUPT(ADC_CONVERSION_COMPLETE_VECT)
{
  static adc* current_adc = NULL;
  static adc* next_adc = NULL;

  SET_DEBUG_LED(0);
  // Set channel for next next conversion
  adc_channel ch = next_next_adc == NULL ? 
    ADC_CHANNEL_GND : adc_get_channel(next_next_adc);
  ADC_SET_CHANNEL(ch);

  //  if (current_adc == NULL) {
  //    // Too slow!
  //    TGL_DEBUG_LED(0);
  //  }

  // Read sample from completed conversion
  if (current_adc != NULL && adc_is_ready(current_adc)) {
    uint16_t sample = ADCW;
    current_adc->next_value += sample;
    current_adc->samples_remaining -= 1;
    if (current_adc->samples_remaining == 0) {
      adc_set_ready(current_adc, false);
    }
  }

  // Notify the ADC process, to add the next ADC to the queue
  process_post_event(&adc_process, EVENT_ADC_CONVERSION_COMPLETE,
		       (process_data_t)current_adc);

  // Shift the queue
  current_adc = next_adc;
  next_adc = next_next_adc;
  next_next_adc = NULL;

  CLR_DEBUG_LED(0);
}
