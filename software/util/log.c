/*
 * log.c
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
 * @file log.c
 * @author Pieter Agten <pieter.agten@gmail.com>
 * @date 09 Aug 2014
 *
 * Logging functions
 */


#include "log.h"
#include <stdbool.h>
#include <stdint.h>

#include "hal/pgmspace.h"

#define STRINGIFY(s) #s

#define LOG_COUNTER_OFF(name)

// Values
uint8_t log_cntrs[_LOG_CNTR_COUNT];


// Names (in program memory)
#define LOG_COUNTER_ON(name) \
  static const char _log_cntr_##name##_name[] PROGMEM = STRINGIFY(name);
#include "log_counters.h"
#undef LOG_COUNTER_ON

static PGM_P const log_cntr_names[] PROGMEM =
{
  #define LOG_COUNTER_ON(name)   _log_cntr_##name##_name,
  #include "log_counters.h"
  #undef LOG_COUNTER_ON
};


void log_cntr_inc(uint8_t index)
{
  if (index < _LOG_CNTR_COUNT) {
    if (log_cntrs[index] < UINT8_MAX) {
      log_cntrs[index] += 1;
    }
  }
}

uint8_t log_cntr_get_value(uint8_t index)
{
  if (index >= _LOG_CNTR_COUNT) {
    return 0;
  }

  return log_cntrs[index];
}


bool log_cntr_get_name(uint8_t index, char* buf, uint8_t n)
{
  if (index >= _LOG_CNTR_COUNT) {
    return false;
  }

  strncpy_P(buf, (PGM_P)pgm_read_word(&(log_cntr_names[index])), n);
  return true;  
}

uint8_t log_cntr_get_nb_counters(void)
{
  return _LOG_CNTR_COUNT;
}

