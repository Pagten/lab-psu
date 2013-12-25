

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "ring_buffer.h"
 
void ring_buffer_init(struct ring_buffer *rb, size_t size)
{
  rb->size = size;
  rb->count = 0;
  rb->latest = 0;
  rb->elems = (uint8_t *)malloc(rb->size * sizeof(uint8_t));
}
 
void ring_buffer_free(struct ring_buffer *rb)
{
  free(rb->elems);
}

bool ring_buffer_full(struct ring_buffer *rb)
{
  return rb->count == rb->size;
}
 
bool ring_buffer_empty (struct ring_buffer *rb)
{
  return rb->count == 0;
}
 
void ring_buffer_put(struct ring_buffer *rb, uint8_t value)
{
  rb->latest = rb->latest + 1 % rb->size;
  rb->elems[rb->latest] = value;
  if (rb->count < rb->size)
    rb->count += 1;
}
 
uint8_t ring_buffer_get_latest(struct ring_buffer *rb, unsigned int i)
{
  if (i >= rb->count) {
    // Index out of bounds
    return 0;
  }

  return rb->elems[rb->latest - i % rb->size];
}
