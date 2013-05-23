/*
 * This code is based on http://www.buxtronix.net/2011/10/rotary-encoders-done-properly.html
 */

#define R_CCW_BEGIN 0x1
#define R_CW_BEGIN 0x2
#define R_START_M 0x3
#define R_CW_BEGIN_M 0x4
#define R_CCW_BEGIN_M 0x5

// Remember: the pin values are inverted, because they are pulled low on a pulse
// 11                    10                01              00
const unsigned char ttable[6][4] = {
  // R_START (00)
  {R_START_M,            R_CW_BEGIN,       R_CCW_BEGIN,    R_START},
  // R_CCW_BEGIN
  {R_START_M | DIR_CCW,  R_START,          R_CCW_BEGIN,    R_START},
  // R_CW_BEGIN
  {R_START_M | DIR_CW,   R_CW_BEGIN,       R_START,        R_START},
  // R_START_M (11)
  {R_START_M,            R_CCW_BEGIN_M,    R_CW_BEGIN_M,   R_START},
  // R_CW_BEGIN_M
  {R_START_M,            R_START_M,        R_CW_BEGIN_M,   R_START | DIR_CW},
  // R_CCW_BEGIN_M
  {R_START_M,            R_CCW_BEGIN_M,    R_START_M,      R_START | DIR_CCW},
};

uint8 rotary_process_step(uint8 prev_state, uint8 pins_state)
{
  uint8 new_state = trans_table[prev_state & 0x0F][pins_state & 0x03];
  return new_state;
}
