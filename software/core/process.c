/*
 * process.c
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
 * @file process.c
 * @author Pieter Agten <pieter.agten@gmail.com>
 * @date 8 feb 2014
 *
 * This file implements processes, based on protothreads. The code is inspired
 * by the processes implementation of Contiki.
 *
 * @see http://dunkels.com/adam/pt
 * @see http://www.contiki-os.org/
 */

#include <stdlib.h>

#include "process.h"

static process* process_list_head;
static process* next_process;


void process_init(void)
{
  process_list_head = NULL;
  next_process = NULL;
}

static bool process_list_contains(process* p)
{
  process* proc = process_list_head;
  while (proc != NULL) {
    if (proc == p) {
      return true;
    }
  }
  return false;
}


process_start_status process_start(process* p)
{
  if (process_list_contains(p)) {
    return PROCESS_START_ALREADY_STARTED;
  }

  // Add p to the process list
  p->next = process_list_head;
  process_list_head = p;

  // Initialize the protothread
  PT_INIT(&p->pt);

  return PROCESS_START_OK;
}

process_stop_status process_stop(process* p)
{
  process** proc = &process_list_head;
  while (*proc != NULL) {
    if (*proc == p) {
      *proc = (*proc)->next;
      return PROCESS_STOP_OK;
    }
  }
  return PROCESS_STOP_NOT_STARTED;
}


void process_execute(void)
{
  if (process_list_head == NULL) {
    // No processes to execute
    return;
  }

  if (next_process == NULL) {
    next_process = process_list_head;
  }
  PROCESS_CALL(next_process);
  next_process = next_process->next;
}
