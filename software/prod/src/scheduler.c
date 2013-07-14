/*
 * scheduler.c
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
 * @file scheduler.c
 * @author Pieter Agten <pieter.agten@gmail.com>
 * @date 29 jun 2013
 *
 */

#include "scheduler.h"

#include "utils/math.h"
#include "utils/log.h"

#ifndef SCHED_TASK_QUEUE_SIZE
#define SCHED_TASK_QUEUE_SIZE  8
#endif

struct task_node
{
  uint16_t tick; // Time at which the task must be executed
  sched_task_t task;
  void* data;
  struct task_node* next;
};

static struct task_node tasks[SCHED_TASK_QUEUE_SIZE];
static volatile struct task_node* waiting_head;
static volatile struct task_node* free_head;
static volatile struct task_node* ready_head;
static volatile struct task_node* ready_tail;

static volatile uint16_t current_tick;
static uint8_t ticks_until_next_isr;


// Prototypes:
static inline void run_queue_put(struct task_node* node);


void sched_init(void)
{
  int i;
  for (i = 0; i < SCHED_TASK_QUEUE_SIZE - 1; ++i) {
    tasks[i].next = &(tasks[i+1]);
  }
  free_head = &(tasks[0]);

  TIMSK2 = _BV(OCIE2A); // Enable OCR2A compare match interrupt
  TCCR2A = 0; // Operate timer 2 in normal mode
  TCCR2B = _BV(CS22) | _BV(CS21) | _BV(CS20); // Enable timer with prescaler set to /1024  
}


sched_status_t sched_schedule(uint16_t ticks, sched_task_t task, void* data)
{
  if (free_head == 0) {
    LOG_WARN("no free task slots in scheduler");
    return SCHED_STATUS_QUEUE_FULL;
  }

  // Remove node from free list
  struct task_node* new_node = free_head;
  free_head = free_head->next;
  
  // Populate the node with data
  new_node->task = task;
  new_node->data = data;
  
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    if (ticks == 0) {
      run_queue_put(new_node);
    } else {
      // Calculate absolute time when task needs to be executed
      uint16_t task_time = current_tick + (TCNT2 - OCR2A) + ticks;

      // Insert node into waiting queue
      struct task_node** node = &waiting_head;
      while (*node != 0 && (*node)->tick < task_time) {
        node = &((*node)->next);
      }
      new_node->tick = task_time
      new_node->next = *node;
      *node = new_node;

      if (ticks < (OCR2A - TCNT2)) {
        OCR2A = TCNT2 + ticks;
      }
    }
  }
}

// Interrupt routine for OCR2A compare match
ISR(TIMER2_COMPA_vect)
{
  // Update current point in time
  current_tick += ticks_until_next_isr;

  struct task_node** node = &waiting_head;
  while (*node != 0 && ((*node)->ticks) <= current_tick) {
    // Add to run queue
    run_queue_put(*node);

    // Remove from waiting list
    *node = *node->next;
    *node->next = 0;
  }
  // Set-up timer for next event
  ticks_until_next_isr = 255;
  if (*node != 0) {
    ticks_until_next_isr = MIN((*node)->ticks - current_tick, 255);
  }
  OCR2A = TCNT2 + ticks_until_next_isr;
}


void sched_start(void) {
  while (true) {
    struct task_node* first = run_head;
    if (first != 0) {
      // Remove first task from run list
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        run_head = first->next;
        if (run_head == 0) run_tail = 0;
      }

      // Execute the task
      first->task(first->data);
      
      // Add it back to the free list
      first->next = free_head;
      free_head = first;
    } else {
      // Idle
    }
  }
}


// Add to end of run queue
static inline void run_queue_put(struct task_node* node)
{
  if (run_tail == 0) {
    run_head = run_tail = node;
  } else {
    run_tail->next = node;
    run_tail = node;
  }
}
