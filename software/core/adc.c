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
#include "util/int.h"

PROCESS(adc_process);

#define SAMPLE_BUFFER_SIZE 8 // Must be at least 3 and preferably a power of 2

#define EVENT_ADC_LIST_CHANGED         (process_event_t)0x00
#define EVENT_ADC_CONVERSION_COMPLETE  (process_event_t)0x01

static adc* adcs;
static adc* next_adc_to_consider;

static volatile uint8_t sample_buffer_head = 0;
static volatile uint8_t sample_buffer_count = 2;
static adc* sample_buffer[SAMPLE_BUFFER_SIZE];

void init_adc(void)
{
  adcs = NULL;
  next_adc_to_consider = NULL;
  sample_buffer_head = 0;
  sample_buffer_count = 2;
  uint8_t i;
  for (i = 0; i < SAMPLE_BUFFER_SIZE; ++i) {
    sample_buffer[i] = NULL;
  }

  ADC_SET_VREF(AREF);
  ADC_SET_ADJUST(RIGHT);
  ADC_SET_AUTO_TRIGGER_SRC(ADC_TRIGGER_FREERUNNING);
  ADC_AUTO_TRIGGER_ENABLE();
  ADC_SET_PRESCALER_DIV(64);
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
  adc->channel = channel;
  adc->resolution = resolution;
  adc->skip = skip;
  adc->process = process;
  return ADC_INIT_OK;
}

/*static inline
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
*/
static inline
void reset_samples_remaining(adc* adc)
{
  adc->samples_remaining = 1 << (2 * adc->resolution);
}

inline
adc_channel adc_get_channel(adc* adc)
{
  return adc->channel;
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
  reset_samples_remaining(adc0);

  // Add new ADC to list
  adc0->next = *a;
  *a = adc0;

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
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    // Prevent the ISR from messing with this ADC structure
    adc0->samples_remaining = 0;
  }

  // Remove ADC from list
  *a = (*a)->next;
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
    adc->value = adc->next_value >> (uint24_t)shift;
  }
}

static inline adc*
find_next_adc_to_queue(void)
{
  static uint8_t period = 0;
  adc* adc = next_adc_to_consider;
  while(adcs != NULL) {
    while(adc != NULL && should_skip(adc, period)) {
      adc = adc->next;
    }
    
    if (adc != NULL) {
      // Found next adc to queue
      next_adc_to_consider = adc->next;
      return adc;
    }

    // Finished current period
    adc = adcs;
    period += 1;
  }
  return NULL;
}

static inline
void fill_sample_buffer(void)
{
  while (sample_buffer_count < SAMPLE_BUFFER_SIZE) {
    adc* next = find_next_adc_to_queue();
    if (next == NULL) {
      return;
    }

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      uint8_t sample_buffer_tail =
	(sample_buffer_head + sample_buffer_count) % SAMPLE_BUFFER_SIZE;
      sample_buffer[sample_buffer_tail] = next;
      sample_buffer_count += 1;
    }
  }
}

static inline void
handle_completed_conversion(adc* adc0)
{
  if (adc0 != NULL && adc0->samples_remaining == 0) {
    // We have enough samples for a full measurement
    set_value(adc0);
    adc0->next_value = 0;
    reset_samples_remaining(adc0);
    if (adc0->process != NULL) {
      process_post_event(adc0->process, ADC_MEASUREMENT_COMPLETED,
			 (process_data_t)adc0);
    }
  }
}

PROCESS_THREAD(adc_process)
{
  PROCESS_BEGIN();

  while(true) {
    PROCESS_WAIT_EVENT();

    fill_sample_buffer();
    if (ev == EVENT_ADC_CONVERSION_COMPLETE) {
      handle_completed_conversion((adc*)data);
    }
  }

  PROCESS_END();
}


INTERRUPT(ADC_CONVERSION_COMPLETE_VECT)
{
  uint8_t current = sample_buffer_head;
  uint8_t count = sample_buffer_count;

  // Set channel for next next conversion
  adc_channel ch;
  adc* next_next_adc = sample_buffer[(current + 2) % SAMPLE_BUFFER_SIZE];
  if (next_next_adc == NULL) {
    // Next next adc to sample is not yet available
    ch = ADC_CHANNEL_GND;   
  } else {
    ch = adc_get_channel(next_next_adc);
  }
  ADC_SET_CHANNEL(ch);

  // Read sample from completed conversion
  adc* current_adc = sample_buffer[current];
  if (current_adc != NULL) {
    if (current_adc->samples_remaining > 0) {
      uint16_t sample = ADCW;
      current_adc->next_value += sample;
      current_adc->samples_remaining -= 1;      
    } else {
      current_adc = NULL;
    }
    sample_buffer[current] = NULL;
  }

  // Notify the ADC process that a sample has been taken
  process_post_event(&adc_process, EVENT_ADC_CONVERSION_COMPLETE,
		     (process_data_t)current_adc);

  // Shift the queue, but make sure we keep a window of at least 2 entries
  if (count > 2) {
    count = count - 1;
  }
  current = (current + 1) % SAMPLE_BUFFER_SIZE;

  sample_buffer_count = count;
  sample_buffer_head = current;
}
