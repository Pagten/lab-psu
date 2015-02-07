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

#define CALIBRATION_NODES 16
#define CAL_PROCESS_NB_STEPS CALIBRATION_NODES

/********* EEPROM *********/
static uint8_t EEMEM EE_adc_to_mvolt_count;
static pwlf_pair EEMEM EE_adc_to_mvolt_pairs[CALIBRATION_NODES];
static uint8_t EEMEM EE_adc_to_mamp_count;
static pwlf_pair EEMEM EE_adc_to_mamp_pairs[CALIBRATION_NODES];
static crc16 EEMEM EE_checksum;
/**************************/

static pwlf adc_to_mvolt = PWLF_INIT(CALIBRATION_NODES);
static pwlf adc_to_mamp  = PWLF_INIT(CALIBRATION_NODES);
static pwlf mvolt_to_dac = PWLF_INIT(CALIBRATION_NODES);
static pwlf mamp_to_dac  = PWLF_INIT(CALIBRATION_NODES);


static bool calibration_running = false;

cal_process_status
cal_process_start(cal_process* p, cal_process_type type)
{
  if (cal_is_process_running()) {
    return CAL_PROCESS_ALREADY_RUNNING;
  }
  if (type >= CAL_PROCESS_TYPE_COUNT) {
    return CAL_PROCESS_INVALID_TYPE;
  }
  if (p->state != CAL_PROCESS_IDLE) {
    return CAL_PROCESS_INVALID_STATE;
  }

  p->type = type;
  p->state = CAL_PROCESS_RUNNING;
  pwlf_clear(&(p->table));
  calibration_running = true;
  return CAL_PROCESS_OK;
}


cal_process_status
cal_process_adc_next(cal_process* p, uint16_t val)
{
  if (p->type != CAL_PROCESS_VOLTAGE_ADC &&
      p->type != CAL_PROCESS_CURRENT_ADC) {
    return CAL_PROCESS_INVALID_TYPE;
  }
  if (p->state != CAL_PROCESS_RUNNING ||
      p->step >= CAL_PROCESS_NB_STEPS) {
    return CAL_PROCESS_INVALID_STATE;
  }


  uint8_t step = cal_process_get_step_number(p);
  uin16_t adc_val = 0; //TODO: read adc value
  if (step > 0) {
    uint16_t prev_val = pwlf_get_y(&(p->table), step - 1);
    if (val <= prev_val) {
      return CAL_PROCESS_INVALID_VALUE;
    }

    uint16_t prev_adc_val = pwlf_get_x(&(p->table), step - 1);
    if (adc_val <= prev_adc_val) {
      return CAL_PROCESS_OUTPUT_ERROR;
    }
  }

  pwlf_add_node(&(p->table), adc_value, val);
  //TODO: set DAC for next step

  return CAL_PROCESS_OK;
}


cal_process_status
cal_process_cancel(cal_process* p)
{
  if (p->state == CAL_PROCESS_IDLE) {
    return CAL_PROCESS_INVALID_STATE;
  }

  p->state = CAL_PROCESS_IDLE;
  pwlf_clear(&(p->table));
  calibration_running = false;
  return CAL_PROCESS_OK;
}


cal_process_status
cal_process_commit(cal_process* p)
{
  if (p->state != CAL_PROCESS_RUNNING ||
      cal_process_get_step_number(p) != CAL_PROCESS_NB_STEPS) {
    return CAL_PROCESS_INVALID_STATE;
  }
  
  pwlf* dst;
  switch (p->type) {
  case CAL_PROCESS_VOLTAGE_ADC:
    dst = &adc_to_mvolt;
    break;
  case CAL_PROCESS_CURRENT_ADC:
    dst = &adc_to_mamps;
    break;
  case CAL_PROCESS_VOLTAGE_DAC:
    dst = &mvolt_to_dac;
    break;
  case CAL_PROCESS_CURRENT_DAC:
    dst = &mamps_to_dac;
    break;
  default:
    p->state = CAL_PROCESS_ERROR;
    return CAL_PROCESS_INVALID_STATE;
  }
  pwlf_copy(&(p->table), dst);
  pwlf_clear(&(p->table));
  p->state = CAL_PROCESS_IDLE;
  calibration_running = false;
  return CAL_PROCESS_OK;
}


inline cal_process_state
cal_process_get_state(cal_process* p)
{
  return p->state;
}


inline uint8_t
cal_process_get_step_number(cal_process* p)
{
  return pwlf_get_count(&(p->table));
}


inline bool
cal_is_process_running()
{
  return calibration_running;
}



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
