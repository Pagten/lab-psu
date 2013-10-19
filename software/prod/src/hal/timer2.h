#ifndef TIMER2_H
#define TIMER2_H

#include <avr/io.h>

#define TMR2_SET_OCA_DISCONNECTED  ( TCCR2A &= ~(_BV(COM2A1) | _BV(COM2A0)) ) // Disconnect OC2A output
#define TMR2_ENABLE_OCA_INTERRUPT  ( TIMSK2 |= _BV(OCIE2A)                    // Enable OCR2A compare match interrupt
#define TMR2_SET_CLOCK_PS1024      ( TCCR2B |= _BV(CS22) | _BV(CS21) | _BV(CS20) ) // Set clock source to clk/1024
#define TMR2_SET_CLOCK_DISABLED    ( TCCR2B &= ~( _BV(CS22) | _BV(CS21) | _BV(CS20)) ) // Disable timer

#define TMR2_OCRA  ( OCR2A )
#define TMR2_CNTR  ( TCNT2 )

#define TMR2_OCA_INTERRUPT_vect  ISR(TIMER2_COMPA_vect)

#endif
