

#include "timer2.h"

struct tmr2 timer2_mock;

void timer2_mock_init()
{
  timer2_mock.TCNT2 = 0;
  timer2_mock.OCR2A = 0;
  timer2_mock.oca_interrupt_enabled = false; 
}

void timer2_mock_set_oca_disconnected()
{

}

void timer2_mock_enable_oca_interrupt()
{
  timer2_mock.oca_interrupt_enabled = true;
}

void timer2_mock_set_mode(char mode)
{

}

void timer2_mock_set_clock_prescaler(int value)
{

}

void timer2_mock_set_clock_disabled()
{

}

void timer2_mock_tick()
{
  timer2_mock.TCNT2 += 1;
  if (timer2_mock.OCR2A == timer2_mock.TCNT2 &&
      timer2_mock.oca_interrupt_enabled) {
    timer2_mock_oca_interrupt_vect();
  }  
}


void timer2_mock_ffw_to_oca()
{
  timer2_mock.TCNT2 = timer2_mock.OCR2A;
  if (timer2_mock.oca_interrupt_enabled) {
    timer2_mock_oca_interrupt_vect();
  }
}
