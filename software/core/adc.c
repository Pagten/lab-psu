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
#include "core/process.h"

PROCESS(adc_process);

static adc* adcs;

void init_adc(void)
{
  adcs = NULL;
  process_start(&adc_process);
}

static bool
is_valid_adc_channel(adc_channel channel)
{
  return 0 <= channel && channel <= 7; 
}

static bool
is_valid_nb_oversamples(adc_oversamples oversamples)
{
  return oversamples == ADC_NO_OVERSAMPLING
    || oversamples == ADC_4X_SAMPLING
    || oversamples == ADC_16X_SAMPLING
    || oversamples == ADC_64X_SAMPLING
    || oversamples == ADC_256X_SAMPLING;
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

adc_init_status
adc_init(adc* adc, adc_channel channel, adc_oversamples oversamples,
	 adc_skip skip)
{
  if (adc_is_enabled(adc)) {
    return ADC_INIT_ALREADY_ENABLED;
  }
  if (! is_valid_adc_channel(channel)) {
    return ADC_INIT_INVALID_CHANNEL;
  }
  if (! is_valid_nb_oversamples(oversamples)) {
    return ADC_INIT_INVALID_NB_OVERSAMPLES;
  }
  if (! is_valid_skip(skip)) {
    return ADC_INIT_INVALID_SKIP;
  }

  adc->value = 0;
  adc->next_value = 0;
  adc->channel = channel;
  adc->oversamples = oversamples;
  adc->skip = skip;
  adc->next = NULL;
  return ADC_INIT_OK;
}

/**
 * Returns whether an adc measurement is enabled.
 *
 * @param adc The ADC measurement structure for which to check if it's enabled
 * @return true if the ADC measurement is enabled, false otherwise.
 */
bool adc_is_enabled(adc* adc)
{
  adc** a = &adcs;
  while (*a != NULL) {
    if (*a == adc) {
      return true;
    }
    if ((*a)->channel > adc->channel) {
      return false;
    }
    a = &((*a)->next);
  }

  return false;
}


/**
 * Enable an ADC measurement.
 *
 * @param adc The ADC measurement structure to enable
 * @return true if the ADC measurement was enabled successfully, false if the
 *         measurement was already enabled.
 */
bool adc_enable(adc* adc)
{
  adc** a = &adcs;
  while (*a != NULL && (*a)->channel <= adc->channel) {
    if (*a == adc) {
      return false;
    }
    a = &((*a)->next);
  }

  adc->next = *a;
  *a = adc;
  return true;
}

/**
 * Disable an ADC measurement.
 *
 * @param adc The ADC measurement structure to disable
 * @return true if the ADC measurement was disabled successfully, false if the
 *         measurement was already disabled.
 */
bool adc_disable(adc* adc)
{
  adc** a = &adcs;
  while (*a != NULL && *a != adc) {
    a = &((*a)->next);
  }

  if (*a == NULL) {
    return false;
  }
  
  *a = (*a)->next;
  return true;
}

/**
 * Returns the latest measurement of an ADC channel.
 *
 * Although the accuracy of the measurement depends on the number of
 * configured oversamples, the measurement is always returned as a 16-bit
 * value. If the accuracy is less than 16 bits, some of the least significant
 * bits will be zero.
 *
 * @param adc The ADC structure of which to return the latest measurement
 * @return The latest measurement of the specified ADC channel.
 */
uint16_t adc_get_measurement(adc* adc)
{
  return adc->value;
}






PROCESS_THREAD(adc_process)
{
  PROCESS_BEGIN();

  PROCESS_END();
}
