#ifndef TIMER2_H
#define TIMER2_H

#include <avr/io.h>
#include "timers.h"

#define timer2_INIT 
#define timer2_oca_SET_DISCONNECTED \
  TCCR2A &= ~(_BV(COM2A1) | _BV(COM2A0)) // Disconnect OC2A output
#define timer2_oca_SET_INTERRUPT_ENABLED \
  TIMSK2 |= _BV(OCIE2A)                  // Enable OCR2A compare match interrupt
#define timer2_SET_MODE_0 \
  TCCR2B &= ~_BV(WGM22); \
  TCCR2A &= ~(_BV(WGM21) | _BV(WGM20)) // Set normal operation mode
#define timer2_SET_CLOCK_ps_1024 \
  TCCR2B |= _BV(CS22) | _BV(CS21) | _BV(CS20) // Set clock source to clk/1024
#define timer2_SET_CLOCK_disabled \
  TCCR2B &= ~( _BV(CS22) | _BV(CS21) | _BV(CS20)) ) // Disable timer

#define timer2_oca_REG      OCR2A
#define timer2_oca_REG_MAX  255
#define timer2_cntr_REG     TCNT2
#define timer2_cntr_REG_MAX 255

#define timer2_oca_INTERRUPT_VECT TIMER2_COMPA_vect

#endif
