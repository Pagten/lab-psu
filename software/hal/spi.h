#ifndef SPI_H
#define SPI_H

#include <avr/io.h>

#define DDR_SPI  DDRB
#define DD_SCK   5
#define DD_MISO  4
#define DD_MOSI  3
#define DD_SS    2

#define SPI_SET_ROLE_MASTER()      SPCR |= _BV(MSTR)
#define SPI_SET_DATA_ORDER_LSB()   SPCR |= _BV(DORD)
#define SPI_SET_DATA_ORDER_MSB()   SPCR &= ~_BV(DORD)
#define SPI_ENABLE()               SPCR |= _BV(SPE)
#define SPI_SET_PIN_DIRS_MASTER()				\
  do {								\
    DDR_SPI |= (_BV(DD_MOSI) | _BV(DD_SCK) | _BV(DD_SS));	\
    DDR_SPI &= ~_BV(DD_MISO);					\
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

#define IS_SPI_INTERRUPT_FLAG_SET() (SPSR & _BV(SPIF))

#endif
