#ifndef PINS_H
#define PINS_H

//Rotary pins must be on port C and pin A must be one higher than B
#define ROT0A_PIN  PORTC3
#define ROT0B_PIN  PORTC2
#define ROT1A_PIN  PORTC1
#define ROT1B_PIN  PORTC0

//DAC regs must be OCR1x
#define DAC0_REG   OCR1A
#define DAC1_REG   OCR1B

#endif
