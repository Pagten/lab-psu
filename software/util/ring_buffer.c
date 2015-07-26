/*
 * ring_buffer.c
 *
 * Copyright 2015 Pieter Agten
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
 * @file ring_buffer.c
 * @author Pieter Agten (pieter.agten@gmail.com)
 * @date 24 Jul 2015
 */

#include "ring_buffer.h"

#include <string.h>

void ringbuf_init(ring_buffer* buf, size_t size, uint8_t* values)
{
  buf->size = size;
  buf->head = size;
  buf->tail = size;
  buf->values = values;
}


bool ringbuf_put(ring_buffer* buf, void* value, uint8_t elsize)
{
  if (ringbuf_full(buf)) {
    return false;
  }

  uint8_t head = buf->head; // Can never be 0 at this point

  head -= 1; // buf->head points to next free location + 1
  memcpy(buf->values + (head * elsize), value, elsize);

  if (head == 0) {
    // Next value should be placed at back of ring buffer
    head = buf->size;
  }

  if (head == buf->tail) {
    // Buffer is full!
    head = 0;
  }

  buf->head = head;

  return true;
}

bool ringbuf_put_byte(ring_buffer* buf, uint8_t value)
{
  if (ringbuf_full(buf)) {
    return false;
  }

  uint8_t head = buf->head; // Can never be 0 at this point

  head -= 1; // buf->head points to next free location + 1
  buf->values[head] = value;

  if (head == 0) {
    // Next value should be placed at back of ring buffer
    head = buf->size;
  }

  if (head == buf->tail) {
    // Buffer is full!
    head = 0;
  }

  buf->head = head;

  return true;
}


void ringbuf_get(ring_buffer* buf, void* dst, uint8_t elsize)
{
  if (ringbuf_empty(buf)) {
    return;
  }

  uint8_t tail = buf->tail; // Can never be 0 at this point

  tail -= 1; // buf->tail points to tail value + 1
  memcpy(dst, buf->values + (tail * elsize), elsize);

  if (tail == 0) {
    // Next tail value is at back of ring buffer
    tail = buf->size;
  }

  if (ringbuf_full(buf)) {
    // If the buffer was full, it now no longer is and the next value can be
    // placed at the location that was removed.
    buf->head = buf->tail;
  }

  buf->tail = tail;
}

uint8_t ringbuf_get_byte(ring_buffer* buf)
{
  if (ringbuf_empty(buf)) {
    return 0;
  }

  uint8_t tail = buf->tail; // Can never be 0 at this point

  tail -= 1; // buf->tail points to tail value + 1
  const uint8_t value = buf->values[tail];

  if (tail == 0) {
    // Next tail value is at back of ring buffer
    tail = buf->size;
  }

  if (ringbuf_full(buf)) {
    // If the buffer was full, it now no longer is and the next value can be
    // placed at the location that was removed.
    buf->head = buf->tail;
  }

  buf->tail = tail;
  return value;
}


void ringbuf_peek(ring_buffer* buf, void* dst, uint8_t elsize)
{
  if (ringbuf_empty(buf)) {
    return;
  }

  memcpy(dst, buf->values + ((buf->tail - 1) * elsize), elsize);
}

uint8_t ringbuf_peek_byte(ring_buffer* buf)
{
  if (ringbuf_empty(buf)) {
    return 0;
  }

  return buf->values[buf->tail - 1];
}

uint8_t ringbuf_free_space(ring_buffer* buf)
{
  if (ringbuf_full(buf)) {
    return 0;
  }

  if (buf->head >= buf->tail) {
    return buf->head - buf->tail;
  } else {
    return buf->size - buf->tail + buf->head;
  }
}

inline bool ringbuf_empty(ring_buffer* buf)
{
  return buf->head == buf->tail;
}

inline bool ringbuf_full(ring_buffer* buf)
{
  return buf->head == 0;
}
