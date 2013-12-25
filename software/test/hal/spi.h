#ifndef SPI_H
#define SPI_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

typedef enum
{
  SPI_ROLE_MASTER,
  SPI_ROLE_SLAVE,
} spi_role;

typedef enum
{
  SPI_DATA_ORDER_LSB,
  SPI_DATA_ORDER_MSB,
} spi_data_order;


void spi_mock_init(size_t transmitted_data_buffer_size);
void spi_mock_set_incoming_data(uint8_t* data, size_t size);
uint8_t spi_mock_get_last_transmitted_data(unsigned int index);

void spi_mock_set_role(spi_role role);
void spi_mock_set_data_order(spi_data_order data_order);
void spi_mock_set_mode(uint8_t cpol, uint8_t cpha);
void spi_mock_set_clock_rate_div(uint8_t div);
void spi_mock_set_enabled(bool enabled);

void spi_mock_write_data_reg(uint8_t val);
uint8_t spi_mock_read_data_reg(void);
void spi_mock_write_status_reg(uint8_t val);
uint8_t spi_mock_read_status_reg(void);


#define SPI_SET_ROLE_MASTER       spi_mock_set_role(SPI_ROLE_MASTER)
#define SPI_SET_DATA_ORDER_LSB    spi_mock_set_data_order(SPI_DATA_ORDER_LSB)
#define SPI_SET_DATA_ORDER_MSB    spi_mock_set_data_order(SPI_DATA_ORDER_MSB)
#define SPI_SET_MODE(_cpol,_cpha) spi_mock_set_mode(_cpol, _cpha)
#define SPI_SET_CLOCK_RATE_div_4  spi_mock_set_clock_rate_div(4)
#define SPI_ENABLE                spi_mock_set_enabled(true)


#define SET_SPI_DATA_REG(x)   spi_mock_write_data_reg(x)
#define GET_SPI_DATA_REG      spi_mock_read_data_reg()
#define SET_SPI_STATUS_REG(x) spi_mock_write_status_reg(x)
#define GET_SPI_STATUS_REG    spi_mock_read_status_reg()

#define SPI_INTERRUPT_FLAG_SET (true)


#endif
