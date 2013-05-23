/*
 * File: dac-test.c
 *
 * Copyright 2013 Pieter Agten
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "pins.h"

static uint16 dac0_value;
static uint16 dac1_value;


void main(void) __attribute__((noreturn));

/**
 * Sets the I/O pin directions.
 */
static inline void initialize_pins(void)
{
  SET_INPUT(ROT0A_PIN);
  SET_INPUT(ROT0B_PIN);
  SET_INPUT(ROT1A_PIN);
  SET_INPUT(ROT1B_PIN);

  SET_OUTPUT(OC1A);
  SET_OUTPUT(OC1B);
}

/**
 * Initializes Timer1 for Fast PWM mode.
 */
static inline void initialize_pwm(void)
{
  // Set initial value to 0 (pins will be high because of output is inverted)
  OCR1A = 0;
  OCR1B = 0;

  // 16-bit period
  ICR1 = 0xFFFF;

  // Enable Fast PWM mode
  TCCR1A = _BV(COM1A1) | _BV(COM1A0) | _BV(COM1B1) | _BV(COM1B0) | _BV(WGM11);
  TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS10);
}


#define STEP_SIZE = 16

/**
 * Pin change interrupt for port C.
 */
ISR(PCINT1_vect)
{
  static uint8 rot0_state;
  static uint8 rot1_state;
  uint8 rot0_pins = (PINB & (_BV(ROT0A_PIN) | _BV(ROT0B_PIN))) >> ROT0B_PIN;
  uint8 rot1_pins = (PINB & (_BV(ROT1A_PIN) | _BV(ROT1B_PIN))) >> ROT1B_PIN;
  rot0_state = rotary_process_step(rot0_state, rot0_pins);
  rot1_state = rotary_process_step(rot1_state, rot1_pins);

  if (rot0_state & DIR_CW && dac0_value <= (0xFFFF - STEP_SIZE)) {
    dac0_value += STEP_SIZE;
    DAC0_REG = dac0_value;
  } else if (rot0_state & DIR_CCW && dac0_value >= STEP_SIZE) {
    dac0_value -= STEP_SIZE;
    DAC0_REG = dac0_value;
  }
  if (rot1_state & DIR_CW && dac1_value <= (0xFFFF - STEP_SIZE)) {
    dac1_value += STEP_SIZE;
    DAC1_REG = dac1_value;
  } else if (rot1_state & DIR_CCW && dac1_value > STEP_SIZE) {
    dac1_value -= STEP_SIZE;
    DAC1_REG = dac1_value;
  }
}


void main(void)
{
  initialize_pins();
  initialize_pwm();
  while(1);
}
