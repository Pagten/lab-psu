#ifndef TIMER2_H
#define TIMER2_H

#include <stdint.h>

struct tmr2 {
  uint8_t OCR2A;
  uint8_t TCNT2;
};

extern struct tmr2 timer2;

void timer2_set_oca_disconnected(void);
void timer2_enable_oca_interrupt(void);
void timer2_set_clock_ps1024(void);
void timer2_set_clock_disabled(void);

void timer2_oca_interrupt_vect(void);


#define TMR2_SET_OCA_DISCONNECTED  timer2_set_oca_disconnected()
#define TMR2_ENABLE_OCA_INTERRUPT  timer2_enable_oca_interrupt()
#define TMR2_SET_CLOCK_PS1024      timer2_set_clock_ps1024()
#define TMR2_SET_CLOCK_DISABLED    timer2_set_clock_disabled()

#define TMR2_OCRA  timer2.OCR2A
#define TMR2_CNTR  timer2.TCNT2

#define TMR2_OCA_INTERRUPT_vect void timer2_oca_interrupt_vect(void)

#endif
