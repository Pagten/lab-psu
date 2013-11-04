

#include "spi.h"

#include <stdint.h>

struct spi spi_mock;

void spi_mock_set_data_reg(uint8_t val)
{
  spi_mock.data_reg = val;
}

uint8_t spi_mock_get_data_reg()
{
  return spi_mock.data_reg;
}

void spi_mock_set_status_reg(uint8_t val)
{
  spi_mock.status_reg = val;
}

uint8_t spi_mock_get_status_reg()
{
  return spi_mock.status_reg;
}
