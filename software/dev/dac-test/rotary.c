/*
 * This code is based on http://www.buxtronix.net/2011/10/rotary-encoders-done-properly.html
 */

#include "config.h"
#include "rotary.h"

#define R_START 0x0
#define R_CCW_BEGIN 0x1
#define R_CW_BEGIN 0x2
#define R_START_M 0x3
#define R_CW_BEGIN_M 0x4
#define R_CCW_BEGIN_M 0x5

//TODO: place in program memory
// 00                    01                10              11
static const uint8_t ttable[6][4] = {
  // R_START (00)
  {R_START,              R_CCW_BEGIN,      R_CW_BEGIN,     R_START_M},
  // R_CCW_BEGIN
  {R_START,              R_CCW_BEGIN,      R_START,        R_START_M | DIR_CCW},
  // R_CW_BEGIN
  {R_START,              R_START,          R_CW_BEGIN,     R_START_M | DIR_CW},
  // R_START_M (11)
  {R_START,              R_CW_BEGIN_M,     R_CCW_BEGIN_M,  R_START_M},
  // R_CW_BEGIN_M
  {R_START | DIR_CW,     R_CW_BEGIN_M,     R_START_M,      R_START_M},
  // R_CCW_BEGIN_M
  {R_START | DIR_CCW,    R_START_M,        R_CCW_BEGIN_M,  R_START_M},
};


uint8_t rotary_process_step(uint8_t prev_state, uint8_t pins_state)
{
  return ttable[prev_state & 0x07][pins_state & 0x03];
}
