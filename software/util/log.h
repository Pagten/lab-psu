/*
 * log.h
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

/**
 * @file log.h
 * @author Pieter Agten <pieter.agten@gmail.com>
 * @date 14 Jul 2013
 *
 * Logging functions
 */

#ifndef LOG_H
#define LOG_H

#include <stdbool.h>
#include <stdint.h>

enum {
  #define LOG_COUNTER_ON(name)   LOG_CNTR_##name,
  #define LOG_COUNTER_OFF(name)  
  #include "log_counters.h"
  #undef LOG_COUNTER_ON
  #undef LOG_COUNTER_OFF

  _LOG_CNTR_COUNT,

  #define LOG_COUNTER_ON(name)
  #define LOG_COUNTER_OFF(name)  LOG_CNTR_##name,
  #include "log_counters.h"
  #undef LOG_COUNTER_ON
  #undef LOG_COUNTER_OFF
};


#define LOG_COUNTER_INC(name)			      \
  do {						      \
    if (LOG_CNTR_##name < _LOG_CNTR_COUNT) {	      \
      log_cntr_inc((uint8_t)LOG_CNTR_##name);	      \
     }						      \
  } while(false)

void log_cntr_inc(uint8_t index);

uint8_t log_cntr_get_value(uint8_t index);

bool log_cntr_get_name(uint8_t index, char* buf, uint8_t n);

uint8_t log_cntr_get_nb_counters(void);

#endif
