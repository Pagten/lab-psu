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
 * @date 08 Feb 2014
 *
 * This file implements processes, based on protothreads. It is inspired by the
 * processes implementation of Contiki.
 *
 * @see http://dunkels.com/adam/pt
 * @see http://www.contiki-os.org
 */

#include <stdbool.h>
#include <stdint.h>

#include "core/clock.h"
#include "core/events.h"
#include "lib/pt/pt.h"

typedef uint8_t process_event_t;
typedef uintptr_t process_data_t;

#define PACK_PROCESS_DATA(b0,b1)  ((process_data_t)((b1 << 8) | b0))
#define UNPACK_PROCESS_DATA0(d)   (d & 0xFF)
#define UNPACK_PROCESS_DATA1(d)   (d >> 8)

typedef struct process {
  struct pt pt;
  PT_THREAD((*thread)(struct process*, process_event_t, process_data_t));
#ifdef PROCESS_STATS
  clock_time_t time;
#endif
} process;


// Empty data
#define PROCESS_DATA_NULL   ((process_data_t)NULL)


/**
 * Create a process, consisting of a thread and a data structure.
 *
 * @param name The name of the process to create
 */
#define PROCESS(name)				\
  PROCESS_THREAD(name);				\
  process name = { .thread = thread_##name }

/**
 * Externally declare a process name, to refer to a process created with the
 * PROCESS macro. This macro can be used in a header file to make a process
 * externally visible.
 *
 * @param name The name of the process to refer to 
 */
#define PROCESS_NAME(name)			\
  extern process name

/**
 * Define the main thread for a process.
 *
 * @param name The name of the process for which to define the main thread
 */
#define PROCESS_THREAD(name)				\
  static PT_THREAD(thread_##name(process* pc,		\
				 process_event_t ev,	\
				 process_data_t data))


/**
 * Define the beginning of a process.
 *
 * This macro must be called at the beginning of a process declaration.
 */
#define PROCESS_BEGIN()				\
  PT_BEGIN(&(pc->pt))


/**
 * Wait while a condition holds.
 *
 * This macro yields the current process while a given condition holds. Other
 * processes execute in the mean time. The condition is periodically checked to
 * determine when to continue. If the condition does not hold when calling this
 * macro, no yield is performed.
 *
 * @param cond The condition during which to wait
 */
#define PROCESS_WAIT_WHILE(cond)		\
  while (cond) {				\
    PROCESS_YIELD();				\
  }


/**
 * Wait until a condition is met.
 *
 * This macro yields the current process until a given condition is met. Other 
 * processes execute in the mean time. The condition is periodically checked to
 * determine when to continue. If the condition holds immediately when calling
 * this macro, no yield is performed.
 *
 * @param cond The condition to wait for
 */
#define PROCESS_WAIT_UNTIL(cond)   \
  PROCESS_WAIT_WHILE(!(cond))


/**
 * Wait until an event is received.
 *
 * This macro yields the current process until an event is received. Other
 * processes execute in the mean time.
 */
#define PROCESS_WAIT_EVENT()	   \
  PT_YIELD(&(pc->pt))


///**
// * Wait until an event is received and a condition is met.
// *
// * This macro yields the current process until an event is received and a given
// * condition is met. Other processes execute in the mean time. The condition is
// * checked whenever the current process receives an event. A yield is performed
// * even if the condition holds immediately when the macro is called.
// */
//#define PROCESS_WAIT_EVENT_UNTIL(cond)	
//  PT_YIELD_UNTIL(&(pc->pt), cond)


/**
 * Wait until an event is received and a condition is met. If an event is
 * received and the condition is not met, then that event is resubmitted.
 *
 * This macro yields the current process until an event is received and a given
 * condition is met. If an event is received and the condition is not met, that
 * event is resubmitted, so the process can respond to it at a later time.
 * Other processes can execute while the process is yielded. The condition is
 * checked whenever the current process receives an event. A yield is performed
 * even if the condition holds immediately when the macro is called.
 */
#define PROCESS_WAIT_EVENT_UNTIL(cond)		  \
  do {						  \
    PT_YIELD(&(pc->pt));			  \
    if (! (cond)) {				  \
      process_post_event(pc, ev, data);		  \
    }						  \
  } while (! (cond))
  

/**
 * Yield the current process.
 *
 * Calling this macro causes the current process to temporarily stop executing,
 * to give other processes some CPU time. The process is resumed when the other
 * processes give up the CPU.
 */
#define PROCESS_YIELD()							\
  do {									\
    process_post_event(pc, PROCESS_EVENT_CONTINUE, PROCESS_DATA_NULL);	\
    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);		\
  } while (0)

/**
 * Define the end of a process.
 *
 * This macro must be called at the end of a process declaration.
 */
#define PROCESS_END()				\
  PT_END(&(pc->pt))


/**
 * Returns a reference to the current process.
 */
#define PROCESS_CURRENT()  pc


typedef enum {
  PROCESS_POST_EVENT_OK,
  PROCESS_POST_EVENT_QUEUE_FULL,
  PROCESS_POST_EVENT_INVALID_PRIORITY,
} process_post_event_status;

typedef enum {
  PROCESS_EVENT_PRIORITY_HIGH,
  PROCESS_EVENT_PRIORITY_NORMAL,
  NB_PROCESS_EVENT_PRIORITIES
} process_event_priority;


/**
 * Initialize the processes module.
 */
void process_init(void);

/** 
 * Start a process. The process will periodically be given CPU time.
 *
 * @param p  The process to start
 */
void process_start(process* p);


/**
 * Asynchronously post an event to a process.
 *
 * It is safe to call this function from an interrupt service routine. Although
 * this function guarantees the event will be delivered to the process, there
 * is of course no guarantee that the process will respond to it.
 *
 * @param p     The process to post the event to
 * @param event The event to post
 * @param data  The data associated with the event
 * @return PROCESS_POST_EVENT_OK if the event was posted successfully, or 
 *        PROCESS_POST_EVENT_QUEUE_FULL if the event could not be posted
 *        because the event queue is full.
 */
process_post_event_status
process_post_event(process* p, process_event_t ev, process_data_t data);

/**
 * Asynchronously post an event to a process, specifying a priority.
 *
 * Events with a higher priority will be dispatched before events with a lower
 * priority and events with the same priority will be dispatched on a FIFO
 * basis.
 *
 * It is safe to call this function from an interrupt service routine. Although
 * this function guarantees the event will be delivered to the process, there
 * is of course no guarantee that the process will respond to it.
 *
 * 
 * @param p     The process to post the event to
 * @param event The event to post
 * @param data  The data associated with the event
 * @param pri   The priority of the event
 * @return PROCESS_POST_EVENT_OK if the event was posted successfully,
 *        PROCESS_POST_EVENT_INVALID_PRIORITY if the given priority is invalid,
 *        or PROCESS_POST_EVENT_QUEUE_FULL if the event could not be posted
 *        because the event queue is full.
 */
process_post_event_status
process_post_priority_event(process* p, process_event_t ev,
			    process_data_t data, process_event_priority pri);



/**
 * Call the next process in line for execution.
 *
 * This function will execute a process if there is one to be executed. Other-
 * wise, this function returns without doing anything. This function is
 * typically called from an infinite loop, thereby executing processes as soon
 * as they are ready to be executed.
 */
void process_execute(void);


/**
 * Return the amount of time spent executing a given process.
 *
 * This is an optional feature, enabled only if the PROCESS_STATS macro is
 * defined. When disabled, this function always returns 0.
 *
 * @param p The process for which to get the total execution time
 * @return The total execution time for the given process.
 */
clock_time_t process_get_time(process* p);


#endif
