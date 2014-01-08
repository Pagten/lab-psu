

#include <stdbool.h>
#include <stdint.h>

#include "util/ring_buffer.h"
#include "spi.h"

struct {
  bool enabled;
  uint8_t data_reg;
  uint8_t status_reg;
} spi_mock;

static uint8_t* incoming_data;
static size_t incoming_data_remaining;
static struct ring_buffer transmitted_data_buffer;

void spi_mock_init(size_t transmitted_data_buffer_size)
{
  spi_mock.enabled = false;
  spi_mock.data_reg = 0;
  spi_mock.status_reg = 0;

  ring_buffer_free(&transmitted_data_buffer);
  ring_buffer_init(&transmitted_data_buffer, transmitted_data_buffer_size);
  incoming_data = 0;
  incoming_data_remaining = 0;
}

void spi_mock_set_incoming_data(uint8_t* data, size_t size)
{
  incoming_data = data;
  incoming_data_remaining = size;
}

uint8_t spi_mock_get_last_transmitted_data(unsigned int index)
{
  return ring_buffer_get_latest(&transmitted_data_buffer, index);
}

void spi_mock_set_pin_dirs_master()
{
 // Not yet implemented
}

void spi_mock_set_role(spi_role role)
{
  // Not yet implemented
}

void spi_mock_set_data_order(spi_data_order data_order)
{ }

void spi_mock_set_mode(uint8_t cpol, uint8_t cpha)
{ }

void spi_mock_set_clock_rate_div(uint8_t div)
{ }

void spi_mock_set_enabled(bool enabled)
{
  spi_mock.enabled = enabled;
}



void spi_mock_write_data_reg(uint8_t val)
{
  if (spi_mock.enabled) {
    ring_buffer_put(&transmitted_data_buffer, val);
    
    if (incoming_data_remaining > 0) {
      spi_mock.data_reg = *incoming_data;
      incoming_data += 1;
      incoming_data_remaining -= 1;
    }
  } else {
    spi_mock.data_reg = val;
  }
}

uint8_t spi_mock_read_data_reg()
{
  return spi_mock.data_reg;
}

void spi_mock_write_status_reg(uint8_t val)
{
  spi_mock.status_reg = val;
}

uint8_t spi_mock_read_status_reg()
{
  return spi_mock.status_reg;
}
