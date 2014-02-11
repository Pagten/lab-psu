/*
 * process.h
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

#ifndef PROCESS_H
#define PROCESS_H

/**
 * @file process.h
 * @author Pieter Agten <pieter.agten@gmail.com>
 * @date 8 feb 2014
 *
 * This file implements processes, based on protothreads.
 */

#include <stdbool.h>

#include "lib/pt/pt.h"

typedef struct process {
  struct pt pt;
  PT_THREAD((*thread)(struct pt*));
  struct process* next;
} process;


#define PROCESS(name)				\
  PROCESS_THREAD(name);				\
  process name = { .next = NULL,		\
		   .thread = thread_##name }

#define PROCESS_NAME(name)			\
  extern process name

#define PROCESS_THREAD(name)			\
  static PT_THREAD(thread_##name(struct pt* pt))

#define PROCESS_INIT()				\
  PT_INIT(pt)

#define PROCESS_BEGIN()				\
  PT_BEGIN(pt)

#define PROCESS_WAIT_UNTIL(cond)		\
  PT_WAIT_UNTIL(pt, cond)

#define PROCESS_WAIT_WHILE(cond)		\
  PT_WAIT_WHILE(pt, cond)

#define PROCESS_EXIT()				\
  PT_EXIT(pt)

#define PROCESS_END()				\
  PT_END(pt)

#define PROCESS_YIELD()				\
  PT_YIELD(pt)


/**
 * Call a process directly.
 *
 * @param p The process to be called.
 * @return true if the process is running, false if it has exited.
 */
static inline 
bool PROCESS_CALL(process* p)
{
  return PT_SCHEDULE((p)->thread(&(p)->pt));
}

typedef enum {
  PROCESS_START_OK,
  PROCESS_START_ALREADY_STARTED,
} process_start_status;

typedef enum {
  PROCESS_STOP_OK,
  PROCESS_STOP_NOT_STARTED,
} process_stop_status;


/**
 * Initialize the processes module.
 */
void process_init(void);

/** 
 * Start a process. The process will periodically be given CPU time.
 *
 * @param p  The process to start
 * @return PROCESS_START_OK if the process was started successfully, or 
 *         PROCESS_START_ALREADY_STARTED if it had already been started 
 *         
 */
process_start_status process_start(process* p);

/**
 * Stop a process.
 *
 * @param p  The process to stop
 * @return PROCESS_STOP_OK if the process was stopped successfully, or
 *         PROCESS_STOP_NOT_STARTED if the process was not started before.
 */
process_stop_status process_stop(process* p);

/**
 * Call the next process in line for execution.
 *
 * This function will execute a process if there is one to be executed. Other-
 * wise, this function returns without doing anything. This function is
 * typically called from an infinite loop, thereby executing processes as soon
 * as they are ready to be executed.
 */
void process_schedule(void);


#endif
