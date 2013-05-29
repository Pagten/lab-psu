#ifndef PINS_H
#define PINS_H

#include "io.h"

#define DEBUG(reg)  BIT(C,5,reg)

//Rotary pins must be on port C
#define ROT0A(reg)  BIT(C,3,reg)
#define ROT0B(reg)  BIT(C,2,reg)
#define ROT1A(reg)  BIT(C,1,reg)
#define ROT1B(reg)  BIT(C,0,reg)

//DAC regs must be OCR1x, DAC pins must be matching output pins
#define DAC0_REG   OCR1A
#define DAC1_REG   OCR1B   
#define DAC0(reg)  BIT(B,1,reg)
#define DAC1(reg)  BIT(B,2,reg)

#endif
