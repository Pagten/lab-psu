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
 * @date 14 jul 2013
 *
 * Logging functions
 */

#ifndef LOG_H
#define LOG_H

typedef enum {
  #define LOG_COUNTER_ON(name)   LOG_CNTR_#name,
  #define LOG_COUNTER_OFF(name)  
  #include "log_counters.h"

  LOG_NB_COUNTERS,

  #define LOG_COUNTER_ON(name)
  #define LOG_COUNTER_OFF(name)  LOG_CNTR_#name,
  #include "log_counters.h"
} log_cntr;


void log_counter_inc(log_cntr cntr);

#define LOG_COUNTER_INC(name)			\
  do {						\
    if (LOG_CNTR_#name <= LOG_NB_COUNTERS) {	\
      log_counter_inc(LOG_CNTR_#name);		\
    }						\
  } while(false)




// in .c

struct log_counter {
  char* name; // TODO: move to program memory
  uint8_t value;
};


#define LOG_COUNTER_ON(name)   { name, 0 },
#define LOG_COUNTER_OFF(name)

static struct log_counter[] =
  {
#include "log_counters.h"
  }




#endif
