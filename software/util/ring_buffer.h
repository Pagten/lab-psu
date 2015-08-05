/*
 * ring_buffer.h
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

#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @file ring_buffer.h
 * @author Pieter Agten (pieter.agten@gmail.com)
 * @date 24 Jul 2015
 *
 * This module provides a memory-efficient ring buffer FIFO queue. The
 * implementation is NOT thread safe!
 */

typedef struct {
  size_t size;
  size_t head;
  size_t tail;
  uint8_t* values;
} ring_buffer;

/**
 * Initialize a ring buffer of a given size.
 *
 * @param buf    The ring buffer data structure to initialize.
 * @param size   The size of the values buffer.
 * @param values The data storage buffer to use for the ring buffer.
 */
void ringbuf_init(ring_buffer* buf, size_t size, uint8_t* values);

/**
 * Add a value to the head of a ring buffer.
 *
 * @param buf    The ring buffer to which to add the value.
 * @param value  The value to add to the ring buffer.
 * @param elsize The size of the elements stored in the buffer.
 * @return true if the value was added succesfully, or false if the ring buffer
 *         is full.
 */
bool ringbuf_put(ring_buffer* buf, void* value, uint8_t elsize);

/**
 * Add a single byte value to the head of a ring buffer.
 *
 * @param buf   The ring buffer to which to add the value.
 * @param value The value to add to the ring buffer.
 * @return true if the value was added succesfully, or false if the ring buffer
 *         is full.
 */
bool ringbuf_put_byte(ring_buffer* buf, uint8_t value);


/**
 * Remove the value at the tail of a ring buffer. No value will be copied into
 * the destination buffer if the ring buffer is empty.
 *
 * @param buf    The ring buffer from which to remove the tail value.
 * @param dst    The buffer into which to write the tail value.
 * @param elsize The size of the elements stored in the buffer.
 */
void ringbuf_get(ring_buffer* buf, void* dst, uint8_t elsize);

/**
 * Remove the single byte value at the tail of a ring buffer.
 *
 * @param buf  The ring buffer from which to remove the tail value.
 * @return The byte value at the tail of the given ring buffer, or 0 if the
 *         buffer is empty.
 */
uint8_t ringbuf_get_byte(ring_buffer* buf);

/**
 * Return the value at the tail of a ring buffer. No value will be copied into
 * the destination buffer if the ring buffer is empty.
 *
 * @param buf    The ring buffer from which to return the tail value.
 * @param dst    The buffer into which to write the tail value.
 * @param elsize The size of the elements stored in the buffer.
 */
void ringbuf_peek(ring_buffer* buf, void* dst, uint8_t elsize);

/**
 * Return the single byte value at the tail of a ring buffer.
 *
 * @param buf  The ring buffer from which to return the tail value.
 * @return The single byte value at the tail of the given buffer, or 0 if the
 *         buffer is empty.
 */
uint8_t ringbuf_peek_byte(ring_buffer* buf);

/**
 * Return the number of free bytes left in the ring buffer.
 *
 * @param buf  The ring buffer of which to return the free bytes left.
 * @return The number of free bytes left in the given ring buffer.
 */
uint8_t ringbuf_free_space(ring_buffer* buf);

/**
 * Return wether a given ring buffer is empty.
 *
 * @param buf The ring buffer to check.
 * @return true if the given ring buffer is empty, or false if not.
 */
bool ringbuf_empty(ring_buffer* buf);

/**
 * Return wether a given ring buffer is full.
 *
 * @param buf The ring buffer to check.
 * @return true if the given ring buffer is full, or false if not.
 */
bool ringbuf_full(ring_buffer* buf);

#endif
