#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

struct ring_buffer {
  size_t size;
  size_t count;
  unsigned int latest;
  uint8_t* elems;
};
 
void ring_buffer_init(struct ring_buffer *rb, size_t size);
 
void ring_buffer_free(struct ring_buffer *rb);

bool ring_buffer_full(struct ring_buffer *rb);
 
bool ring_buffer_empty (struct ring_buffer *rb);
 
void ring_buffer_put(struct ring_buffer *rb, uint8_t value);
 
uint8_t ring_buffer_get_latest(struct ring_buffer *rb, unsigned int i);

#endif
