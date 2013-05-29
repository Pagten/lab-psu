/*
 * File: dac-test.c
 *
 * Copyright 2013 Pieter Agten
 */

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "pins.h"
#include "rotary.h"

/*
 * Fuses:
 *
 *  L: 0xF7
 *  H: 0xD9
 *  E: 0x07
 *
 *  avrdude:
 *   -U lfuse:w:0xf7:m -U hfuse:w:0xd9:m -U efuse:w:0x07:m
 */

void main(void) __attribute__((noreturn));

/**
 * Sets the I/O pin directions.
 */
static inline void initialize_pins(void)
{
  DEBUG(DDR) = OUTPUT;

  ROT0A(DDR) = INPUT;
  ROT0B(DDR) = INPUT;
  ROT1A(DDR) = INPUT;
  ROT1B(DDR) = INPUT;

  DAC0(DDR) = OUTPUT;
  DAC1(DDR) = OUTPUT;
}

#define DAC_TOP 0x0FFF //12-bit

/**
 * Initializes Timer1 for Fast PWM mode.
 */
static inline void initialize_pwm(void)
{
  // Set initial value to 0
  DAC0_REG = 0;
  DAC1_REG = 0;

  // Set period
  ICR1 = DAC_TOP;

  // Enable Fast PWM mode
  TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(WGM11);
  TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS10);
}


#define STEP_SIZE 64

/**
 * Pin change interrupt for port C.
 */
ISR(PCINT1_vect)
{
  static uint8_t rot0_state;
  //static uint8_t rot1_state;
  uint8_t rot0_pins = (ROT0A(READ) << 1) | ROT0B(READ);
  //uint8_t rot1_pins = (ROT1A(READ) << 1) | ROT1B(READ);
  rot0_state = rotary_process_step(rot0_state, rot0_pins);
  //rot1_state = rotary_process_step(rot1_state, rot1_pins);

  //Toggle pin (TODO: use proper pin name!):
  //PORTC |= _BV(PC5);

  if (rot0_state & DIR_CW) {
    if (DAC0_REG < DAC_TOP - STEP_SIZE) {
      DAC0_REG += STEP_SIZE;
    } else {
      DAC0_REG = DAC_TOP + 1;
    }
  } else if (rot0_state & DIR_CCW) {
    if (DAC0_REG > STEP_SIZE) {
      DAC0_REG -= STEP_SIZE;
    } else {
      DAC0_REG = 0x0000;
    }
  }
  /*if (rot1_state & DIR_CW) {
    if (DAC1_REG < 0xFFFF - STEP_SIZE) {
      DAC1_REG += STEP_SIZE;
    } else {
      DAC0_REG = 0xFFFF;
    }
  } else if (rot1_state & DIR_CCW) {
    if (DAC1_REG > STEP_SIZE) {
      DAC1_REG -= STEP_SIZE;
    } else {
      DAC1_REG = 0x0000;
    }
    }*/
}


void main(void)
{
  initialize_pins();
  initialize_pwm();

  PCICR = _BV(PCIE1); // Enable Pin Change Interrupt 1
  PCMSK1 = _BV(PCINT8) | _BV(PCINT9) | _BV(PCINT10) | _BV(PCINT11);
  sei(); // Enable interrupts

  while(1);
}
