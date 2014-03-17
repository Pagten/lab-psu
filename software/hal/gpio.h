/*
 * gpio.h
 *
 * Copyright 2013 Pieter Agten
 *
 * This file is part of the lab-psu firmware.
 *
 * The firmware is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The firmware is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the firmware.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GPIO_H
#define GPIO_H

/**
 * @file gpio.h
 * @author Pieter Agten <pieter.agten@gmail.com>
 * @date 7 dec 2013
 *
 * This file provides general purpose i/o bit manipulation macros. These macros
 * allow you to assign a symbol to a specific pin and to use that symbol to
 * manipulate the pin's value.
 *
 * For instance:
 * #define MYSYMBOL  C,1   //defines MYSYMBOL as pin 1 of port C
 *
 * SET_PIN_DIR_INPUT(MYSYMBOL);
 * char val = GET_PIN(MYSYMBOL);
 *
 * SET_PIN_DIR_OUTPUT(MYSYMBOL);
 * SET_PIN(MYSYMBOL);  
 */

#include "devices.h"
#include <avr/io.h>


/**
 * Macros for pin symbols
 *
 * The macros below allow you to assign a symbol to a specific pin and to use that
 * symbol to manipulate the pin's value.
 *
 * For instance:
 * #define MYSYMBOL  C,1   //defines MYSYMBOL as pin 1 of port C
 *
 * SET_PIN_DIR_INPUT(MYSYMBOL);
 * char val = GET_PIN(MYSYMBOL);
 *
 * SET_PIN_DIR_OUTPUT(MYSYMBOL);
 * SET_PIN(MYSYMBOL);  
 */

#define GET_PORT(pb)             PORT(pb)
#define GET_BIT(pb)              B(pb)

#define GET_PIN(pb)              GET_PORT_BIT(PIN(pb),B(pb))
#define SET_PIN(pb)              SET_PORT_BIT(PORT(pb),B(pb))
#define CLR_PIN(pb)              CLR_PORT_BIT(PORT(pb),B(pb)) 
#define TGL_PIN(pb)              TGL_PORT_BIT(PIN(pb),B(pb)) 

#define GET_PIN_DIR(pb)          GET_PORT_BIT(DDR(pb),B(pb))
#define SET_PIN_DIR_OUTPUT(pb)   SET_PORT_BIT(DDR(pb),B(pb))
#define SET_PIN_DIR_INPUT(pb)    CLR_PORT_BIT(DDR(pb),B(pb))
#define TGL_PIN_DIR(pb)          TGL_PORT_BIT(DDR(pb),B(pb))

#define PC_INTERRUPT_VECT(pb)    PCINT_vect(pb)
#define PC_INTERRUPT_ENABLE(pb)			\
  do {						\
    SET_PORT_BIT(PCICR,PCIE(pb));		\
    SET_PORT_BIT(PCMSK(pb),PCINT(pb));		\
  } while(0);
#define PC_INTERRUPT_DISABLE(pb) CLR_PORT_BIT(PCMSK(pb),PCINT(pb))



/**
 * Macros for pointers to I/O ports
 *
 * The macros below allow you to manipulate pins based on pointers to the
 * corresponding port register.
 */
typedef volatile uint8_t* port_ptr;

#define P_SET_PINS(port, mask)              *(port) |= (mask)
#define P_CLR_PINS(port, mask)              *(port) &= ~(mask)

#define P_GET_VAL(port)                     (*GET_PIN_REG(port))
#define P_SET_VAL(port, value)              *(port) = (value)

#define P_SET_PINS_DIR_OUTPUT(port, mask)   *GET_DDR_REG(port) |= (mask)
#define P_SET_PINS_DIR_INPUT(port, mask)    *GET_DDR_REG(port) &= ~(mask)





//*****************************************************************************
// THE MACROS BELOW ARE FOR INTERNAL USAGE ONLY, THEY ARE NOT TO BE USED
// DIRECTLY IN USER CODE
//*****************************************************************************
#define P(p,b)                  (p)
#define B(p,b)                  (b)

#define PORT(p,b)               (PORT ## p) 
#define PIN(p,b)                (PIN ## p) 
#define DDR(p,b)                (DDR ## p)

#define GET_PORT_BIT(p,b)       (((p) & _BV(b)) != 0) 
#define SET_PORT_BIT(p,b)       ((p) |= _BV(b)) 
#define CLR_PORT_BIT(p,b)       ((p) &= ~_BV(b)) 
#define TGL_PORT_BIT(p,b)       ((p) |= _BV(b))

// Lookups in devices.h
#define PCINT(p,b)              PCINT_ ## p ## b
#define PCINT_vect(p,b)         PCINT_vect_ ## p ## b
#define PCIE(p,b)               PCIE_ ## p ## b
#define PCMSK(p,b)              PCMSK_ ## p ## b



#define GET_DDR_REG(port) ((port) - 1)
#define GET_PIN_REG(port) ((port) - 2)


#endif
