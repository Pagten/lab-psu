#ifndef SPI_H
#define SPI_H

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
  SPI_DUMMY_MASTER,
  SPI_DUMMY_SLAVE,
} spi_dummy_role;

typedef enum
{
  SPI_DUMMY_LSB,
  SPI_DUMMY_MSB,
} spi_dummy_data_order;

struct spi {
  spi_dummy_role role;
  spi_dummy_data_order data_order;
  bool cpol;
  bool cpha;
  uint8_t clock_rate_div;
  bool enabled;
  uint8_t data_reg;
  uint8_t status_reg;
};

extern struct spi spi_mock;


#define SPI_SET_ROLE_MASTER       ( spi_mock.role = SPI_DUMMY_MASTER )
#define SPI_SET_DATA_ORDER_LSB    ( spi_mock.data_order = SPI_DUMMY_LSB )
#define SPI_SET_DATA_ORDER_MSB    ( spi_mock.data_order = SPI_DUMMY_MSB )
#define SPI_SET_MODE(_cpol,_cpha)   spi_mock.cpol = _cpol; spi_mock.cpha = _cpha
#define SPI_SET_CLOCK_RATE_div_4  ( spi_mock.clock_rate_div = 4 )
#define SPI_ENABLE                ( spi_mock.enabled = true )


#define SPI_DATA_REG    spi_mock.data_reg
#define SPI_STATUS_REG  spi_mock.status_reg 

#define SPI_INTERRUPT_FLAG_SET (true)


#endif
