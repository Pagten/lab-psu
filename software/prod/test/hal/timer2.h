#ifndef TIMER2_H
#define TIMER2_H

#include <stdbool.h>
#include <stdint.h>
#include "hal/timers.h"


struct tmr2 {
  uint8_t OCR2A;
  uint8_t TCNT2;
  bool oca_interrupt_enabled;
};

extern struct tmr2 timer2_mock;

void timer2_mock_init(void);
void timer2_mock_set_oca_disconnected(void);
void timer2_mock_enable_oca_interrupt(void);
void timer2_mock_set_mode(char);
void timer2_mock_set_clock_prescaler(int);
void timer2_mock_set_clock_disabled(void);
void timer2_mock_oca_interrupt_vect(void);

void timer2_mock_tick(void);
void timer2_mock_ffw_to_oca(void);

#define timer2_INIT                      timer2_mock_init()
#define timer2_oca_SET_DISCONNECTED      timer2_mock_set_oca_disconnected()
#define timer2_oca_SET_INTERRUPT_ENABLED timer2_mock_enable_oca_interrupt()
#define timer2_SET_MODE_0                timer2_mock_set_mode(0)
#define timer2_SET_CLOCK_ps_1024         timer2_mock_set_clock_prescaler(1024)
#define timer2_SET_CLOCK_disabled        timer2_mock_set_clock_disabled()

#define timer2_oca_REG      timer2_mock.OCR2A
#define timer2_oca_REG_MAX  255
#define timer2_cntr_REG     timer2_mock.TCNT2
#define timer2_cntr_REG_MAX 255

#define timer2_oca_INTERRUPT_VECT void timer2_mock_oca_interrupt_vect(void)

#endif
