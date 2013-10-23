#ifndef TIMERS_H
#define TIMERS_H


#define CONCAT2(one,two) one ## _ ## two
#define CONCAT3(one,two,three) one ## _ ## two ## _ ## three

#define SET_OUTPUT_DISCONNECTED(tmr,output)  CONCAT3(tmr,output,SET_DISCONNECTED)
#define ENABLE_INTERRUPT(tmr,interrupt)      CONCAT3(tmr,interrupt,SET_ENABLED)
#define SET_CLOCK(tmr,value)                 CONCAT3(tmr,SET_CLOCK,value)

#define TMR_REG(tmr,name)      CONCAT3(tmr,name,REG)
#define TMR_REG_MAX(tmr,name)  CONCAT3(tmr,name,REG_MAX)

#define TMR_INTERRUPT_VECT(tmr, interrupt)  CONCAT3(tmr,interrupt,INTERRUPT_VECT)

#endif
