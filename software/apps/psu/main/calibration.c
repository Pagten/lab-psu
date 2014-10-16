/*
 * calibration.c
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
 * @file calibration.c
 * @author Pieter Agten (pieter.agten@gmail.com)
 * @date 12 Oct 2014
 */

#include "calibration.h"
#include "calibration_defaults.h"

#include <stdbool.h>
#include <stdint.h>

#include "core/crc16.h"
#include "core/eeprom.h"
#include "core/pwlf.h"

#include "util/debug.h"
#include <math.h>

#define ADC_TO_VOLTAGE_NODES 16
#define ADC_TO_CURRENT_NODES 16
#define VOLTAGE_TO_DAC_NODES 16
#define CURRENT_TO_DAC_NODES 16


/********* EEPROM *********/
static uint8_t EEMEM EE_adc_to_mvolt_count;
static pwlf_pair EEMEM EE_adc_to_mvolt_pairs[ADC_TO_VOLTAGE_NODES];
static uint8_t EEMEM EE_adc_to_mamp_count;
static pwlf_pair EEMEM EE_adc_to_mamp_pairs[ADC_TO_CURRENT_NODES];
static crc16 EEMEM EE_checksum;
/**************************/

static pwlf adc_to_mvolt = PWLF_INIT(ADC_TO_VOLTAGE_NODES);
static pwlf adc_to_mamp  = PWLF_INIT(ADC_TO_CURRENT_NODES);
static pwlf mvolt_to_dac = PWLF_INIT(VOLTAGE_TO_DAC_NODES);
static pwlf mamp_to_dac  = PWLF_INIT(CURRENT_TO_DAC_NODES);


void cal_load_defaults(void)
{
  // ADC to voltage
  pwlf_clear(&adc_to_mvolt);
  pwlf_add_node(&adc_to_mvolt, ADC_TO_MVOLT_MIN);
  pwlf_add_node(&adc_to_mvolt, ADC_TO_MVOLT_MAX);

  // ADC to current
  pwlf_clear(&adc_to_mamp);
  pwlf_add_node(&adc_to_mamp, ADC_TO_MAMP_MIN);
  pwlf_add_node(&adc_to_mamp, ADC_TO_MAMP_MAX);

  // Voltage to DAC
  pwlf_clear(&mvolt_to_dac);
  pwlf_add_node(&mvolt_to_dac, MVOLT_TO_DAC_MIN);
  pwlf_add_node(&mvolt_to_dac, MVOLT_TO_DAC_MAX);

  // Current to DAC
  pwlf_clear(&mamp_to_dac);
  pwlf_add_node(&mamp_to_dac, MAMP_TO_DAC_MIN);
  pwlf_add_node(&mamp_to_dac, MAMP_TO_DAC_MAX);  
}


bool cal_load_from_eeprom(void)
{
  crc16 crc;
  crc16_init(&crc);

  eeprom_read_block_crc(&adc_to_mvolt.count, &EE_adc_to_mvolt_count,
			sizeof(adc_to_mvolt.count), &crc);
  eeprom_read_block_crc(&adc_to_mvolt.values, &EE_adc_to_mvolt_pairs,
			sizeof(EE_adc_to_mvolt_pairs), &crc);
  eeprom_read_block_crc(&adc_to_mamp.count, &EE_adc_to_mamp_count,
			sizeof(adc_to_mamp.count), &crc);
  eeprom_read_block_crc(&adc_to_mamp.values, &EE_adc_to_mamp_pairs,
			sizeof(EE_adc_to_mamp_pairs), &crc);

  // Check CRC checksum
  crc16 saved_crc;
  eeprom_read_block(&saved_crc, &EE_checksum, sizeof(saved_crc));
  return crc16_equal(&crc, &saved_crc);
}


bool cal_verify_eeprom(void)
{
  crc16 crc;
  unsigned int i;
  crc16_init(&crc);

  // Recalculate checksum
  for (i = 0; i < sizeof(EE_adc_to_mvolt_count); ++i) {
    crc16_update(&crc, eeprom_read_byte((&EE_adc_to_mvolt_count) + i));
  }
  for (i = 0; i < sizeof(EE_adc_to_mvolt_pairs); ++i) {
    crc16_update(&crc, eeprom_read_byte(((void *)EE_adc_to_mvolt_pairs) + i));
  }
  for (i = 0; i < sizeof(EE_adc_to_mamp_count); ++i) {
    crc16_update(&crc, eeprom_read_byte((&EE_adc_to_mamp_count) + i));
  }
  for (i = 0; i < sizeof(EE_adc_to_mamp_pairs); ++i) {
    crc16_update(&crc, eeprom_read_byte(((void *)EE_adc_to_mamp_pairs) + i));
  }

  // Check CRC checksum
  crc16 saved_crc;
  eeprom_read_block(&saved_crc, &EE_checksum, sizeof(saved_crc));
  return crc16_equal(&crc, &saved_crc);
}


void cal_save_to_eeprom(void)
{
  crc16 crc;
  crc16_init(&crc);

  eeprom_update_block_crc(&adc_to_mvolt.count, &EE_adc_to_mvolt_count,
			  sizeof(EE_adc_to_mvolt_count), &crc);
  eeprom_update_block_crc(&adc_to_mvolt.values, &EE_adc_to_mvolt_pairs,
			  sizeof(EE_adc_to_mvolt_pairs), &crc);
  eeprom_update_block_crc(&adc_to_mamp.count, &EE_adc_to_mamp_count,
			  sizeof(EE_adc_to_mamp_count), &crc);
  eeprom_update_block_crc(&adc_to_mamp.values, &EE_adc_to_mamp_pairs,
			  sizeof(EE_adc_to_mamp_pairs), &crc);

  // Save checksum
  eeprom_update_block(&crc, &EE_checksum, sizeof(EE_checksum));
}


inline
int16_t cal_adc_to_mvolt(uint16_t adc)
{
  return pwlf_value(&adc_to_mvolt, adc);
}

inline
int16_t cal_adc_to_mamp(uint16_t adc)
{
  return pwlf_value(&adc_to_mamp, adc);
}

inline
uint16_t cal_mvolt_to_dac(uint16_t mvolt)
{
  return pwlf_value(&mvolt_to_dac, mvolt);
}

inline
uint16_t cal_mamp_to_dac(uint16_t mamp)
{
  return pwlf_value(&mamp_to_dac, mamp);
}
