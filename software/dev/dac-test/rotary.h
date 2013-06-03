#ifndef ROTARY_H
#define ROTARY_H

#include <stdint.h>

#define DIR_CW 0x10
#define DIR_CCW 0x20

uint8_t rotary_process_step(uint8_t prev_state, uint8_t pins_state);

#endif
