/*
 * scheduler.h
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
 * @file scheduler.h
 * @author Pieter Agten <pieter.agten@gmail.com>
 * @date 29 jun 2013
 *
 * The task scheduler.
 */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>

typedef enum
{
  SCHED_OK,
  SCHED_QUEUE_FULL,
} sched_schedule_status;

typedef enum
{
  SCHED_TASK_EXECUTED,
  SCHED_IDLE,
} sched_exec_status;

typedef uint16_t ticks_t;

/**
 * The number of timer ticks per millisecond.
 */
#define SCHED_TICKS_MS  (1) //TODO: fill in!

/**
 * Type of a task that can be scheduled.
 */
typedef void (*sched_task_t)(void* data);

/**
 * Initialize the task scheduler. Must be called once before calling any
 * other functions.
 */
void sched_init(void);

/**
 * Schedule a task for execution in the near future.
 *
 * @param ticks The time (in timer ticks) to wait before executing the task
 * @param task  The task to execute
 * @param data  A pointer to opaque data that will be passed to the task
 * @return SCHED_OK if the task was scheduled correctly, SCHED_QUEUE_FULL if
 *         the task scheduling queue was full.
 */
sched_schedule_status sched_schedule(ticks_t ticks, sched_task_t task, void* data);


/**
 * Execute a scheduled task, if one is ready to be executed.
 *
 * This function will execute one scheduled task that is ready to be executed,
 * if there is such a task. Otherwise, this function returns without doing
 * anything. This function is to be called in an infinite loop, thereby executing
 * tasks as soon as they are ready to be executed. The function returns
 * SCHED_TASK_EXECUTED if a task was executed and SCHED_IDLE otherwise. 
 */
sched_exec_status sched_exec(void);
 
#endif
