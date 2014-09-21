/*
 * spi.h
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

#ifndef SPI_H
#define SPI_H

#include <avr/io.h>

#define DDR_SPI  DDRB
#define DD_SCK   5
#define DD_MISO  4
#define DD_MOSI  3
#define DD_SS    2

#define SPI_SS_PIN B,2

#define SPI_SET_ROLE_MASTER()      SPCR |= _BV(MSTR)
#define SPI_SET_ROLE_SLAVE()       SPCR &= _BV(MSTR)
#define SPI_SET_DATA_ORDER_LSB()   SPCR |= _BV(DORD)
#define SPI_SET_DATA_ORDER_MSB()   SPCR &= ~_BV(DORD)
#define SPI_TC_INTERRUPT_ENABLE()  SPCR |= _BV(SPIE)
#define SPI_TC_INTERRUPT_DISABLE() SPCR &= ~_BV(SPIE)
#define SPI_ENABLE()               SPCR |= _BV(SPE)
#define SPI_SET_PIN_DIRS_MASTER()				\
  do {								\
    DDR_SPI |= (_BV(DD_MOSI) | _BV(DD_SCK) | _BV(DD_SS));	\
    DDR_SPI &= ~_BV(DD_MISO);					\
  } while(0)
#define SPI_SET_PIN_DIRS_SLAVE()				\
  do {								\
    DDR_SPI &= ~(_BV(DD_MOSI) | _BV(DD_SCK) | _BV(DD_SS));	\
    DDR_SPI |= _BV(DD_MISO);					\
  } while(0)
#define SPI_SET_MODE(cpol,cpha)                         \
  do {                                                  \
    SPCR = cpol ? SPCR | _BV(CPOL) : SPCR & ~_BV(CPOL); \
    SPCR = cpha ? SPCR | _BV(CPHA) : SPCR & ~_BV(CPHA); \
  } while(0)
#define SPI_SET_CLOCK_RATE_DIV_4()    \
  do {                                \
    SPCR &= ~(_BV(SPR1) | _BV(SPR0)); \
    SPSR &= ~_BV(SPI2X);              \
  } while(0)
#define SPI_SET_CLOCK_RATE_DIV_8()    \
  do {                                \
    SPCR |= _BV(SPR0);		      \
    SPCR &= ~_BV(SPR1);		      \
    SPSR |= _BV(SPI2X);		      \
  } while(0)
#define SPI_SET_CLOCK_RATE_DIV_16()   \
  do {                                \
    SPCR |= _BV(SPR0);		      \
    SPCR &= ~_BV(SPR1);		      \
    SPSR &= ~_BV(SPI2X);	      \
  } while(0)
#define SPI_SET_CLOCK_RATE_DIV_32()   \
  do {                                \
    SPCR &= ~_BV(SPR0);		      \
    SPCR |= _BV(SPR1);		      \
    SPSR |= _BV(SPI2X);		      \
  } while(0)
#define SPI_SET_CLOCK_RATE_DIV_64()   \
  do {                                \
    SPCR |= (_BV(SPR1) | _BV(SPR0));  \
    SPSR |= _BV(SPI2X);		      \
  } while(0)
#define SPI_SET_CLOCK_RATE_DIV_128()  \
  do {                                \
    SPCR |= (_BV(SPR1) | _BV(SPR0));  \
    SPSR &= ~_BV(SPI2X);	      \
  } while(0)


#define SPI_SET_DATA_REG(val)  SPDR = val
#define SPI_GET_DATA_REG()     SPDR
//#define SPI_GET_STATUS_REG()   SPSR
#define SPI_CLEAR_FLAGS()	   \
  do {				   \
    uint8_t __attribute__((unused)) dummy0 = SPSR;	   \
    uint8_t __attribute__((unused)) dummy1 = SPDR;	   \
  } while(0)
#define IS_SPI_INTERRUPT_FLAG_SET()       (SPSR & _BV(SPIF))
#define IS_SPI_WRITE_COLLISION_FLAG_SET() (SPSR & _BV(WCOL))

// Transfer complete interrupt
#define SPI_TC_VECT  SPI_STC_vect

#endif
