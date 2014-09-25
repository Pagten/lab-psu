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

#ifndef CORE_ADC_H
#define CORE_ADC_H

/**
 * @file adc.h
 * @author Pieter Agten <pieter.agten@gmail.com>
 * @date 19 Sep 2014
 *
 * The ADC module provides an API to read out the MCU's analog to digital
 * converters.
 */

#include <stdbool.h>
#include <stdint.h>
#include "core/process.h"
#include "hal/adc.h"

struct adc {
  uint16_t value;
  uint16_t next_value;
  uint8_t flags_channel; // 4 LSBs used for channel
  uint8_t oversamples;
  uint8_t oversamples_remaining;
  uint8_t skip;
  process* process;
  struct adc* next;
};
typedef struct adc adc;


typedef enum {
  ADC_NO_OVERSAMPLING = 0,
  ADC_4X_SAMPLING = 3,
  ADC_16X_SAMPLING = 15,
  ADC_64X_SAMPLING = 63,
  ADC_256X_SAMPLING = 255,
} adc_oversamples;

typedef enum {
  ADC_SKIP_0 = 0,
  ADC_SKIP_1 = 1,
  ADC_SKIP_3 = 3,
  ADC_SKIP_7 = 7,
  ADC_SKIP_15 = 15,
} adc_skip;

typedef enum {
  ADC_INIT_OK,
  ADC_INIT_ALREADY_IN_LIST,
  ADC_INIT_INVALID_CHANNEL,
  ADC_INIT_INVALID_NB_OVERSAMPLES,
  ADC_INIT_INVALID_SKIP,
} adc_init_status;


/**
 * Initialize the ADC module.
 */
void init_adc(void);


/**
 * Initialize an ADC structure.
 *
 * @param adc         The ADC structure to initialize
 * @param channel     The ADC channel to measure
 * @param oversamples The number of oversamples to perform each measurement
 * @param skip        The number of sample slots to skip each period
 * @param process     The process to notify when a new measurement is
 *                    available (can be NULL)
 * @return ADC_INIT_OK if the structure was initialized successfully, 
 *         ADC_INIT_ALREADY_IN_LIST if the specified ADC structure is already
 *         enabled, ADC_INIT_INVALID_CHANNEL if the specified channel is
 *         invalid, ADC_INIT_INVALID_NB_OVERSAMPLES if the specified number of
 *         oversamples is invalid, or ADC_INIT_INVALID_SKIP if the specified
 *         number of sample slots to skip is invalid.
 */
adc_init_status
adc_init(adc* adc_, adc_channel channel, adc_oversamples oversamples,
	 adc_skip skip, process* process); 

/**
 * Returns whether an ADC measurement is enabled.
 *
 * @param adc The ADC measurement structure for which to check if it's enabled
 * @return true if the ADC measurement is enabled, false otherwise.
 */
bool adc_is_enabled(adc* adc);

/**
 * Returns the channel of an ADC measurement structure.
 *
 * @param adc The ADC measurement structure for which to get the channel.
 * @return The channel of the given ADC measurement structure.
 */
adc_channel adc_get_channel(adc* adc);

/**
 * Enable an ADC measurement.
 *
 * @param adc The ADC measurement structure to enable
 * @return true if the ADC measurement was enabled successfully, false if the
 *         measurement was already enabled.
 */
bool adc_enable(adc* adc);

/**
 * Disable an ADC measurement.
 *
 * @param adc The ADC measurement structure to disable
 * @return true if the ADC measurement was disabled successfully, false if the
 *         measurement was already disabled.
 */
bool adc_disable(adc* adc);

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
uint16_t adc_get_measurement(adc* adc);

#endif
