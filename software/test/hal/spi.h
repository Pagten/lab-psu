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

typedef enum
{
  SPI_INTERRUPT_TC,
} spi_interrupt;

void spi_mock_init(size_t transmitted_data_buffer_size);
void spi_mock_set_incoming_data(uint8_t* data, size_t size);
uint8_t spi_mock_get_last_transmitted_data(unsigned int index);
unsigned int spi_mock_get_nb_bytes_transmitted(void);

void spi_mock_set_pin_dirs_master(void);
void spi_mock_set_pin_dirs_slave(void);
void spi_mock_set_role(spi_role role);
void spi_mock_set_data_order(spi_data_order data_order);
void spi_mock_set_interrupt_enabled(spi_interrupt i, bool v);
void spi_mock_set_mode(uint8_t cpol, uint8_t cpha);
void spi_mock_set_clock_rate_div(uint8_t div);
void spi_mock_set_enabled(bool enabled);

void spi_mock_write_data_reg(uint8_t val);
uint8_t spi_mock_read_data_reg(void);
void spi_mock_write_status_reg(uint8_t val);
uint8_t spi_mock_read_status_reg(void);

#define SPI_SET_PIN_DIRS_MASTER()   spi_mock_set_pin_dirs_master()
#define SPI_SET_PIN_DIRS_SLAVE()    spi_mock_set_pin_dirs_slave()
#define SPI_SET_ROLE_MASTER()       spi_mock_set_role(SPI_ROLE_MASTER)
#define SPI_SET_ROLE_SLAVE()        spi_mock_set_role(SPI_ROLE_SLAVE)
#define SPI_SET_DATA_ORDER_LSB()    spi_mock_set_data_order(SPI_DATA_ORDER_LSB)
#define SPI_SET_DATA_ORDER_MSB()    spi_mock_set_data_order(SPI_DATA_ORDER_MSB)
#define SPI_TC_INTERRUPT_ENABLE()   \
  spi_mock_set_interrupt_enabled(SPI_INTERRUPT_TC, true);
#define SPI_TC_INTERRUPT_DISABLE()  \
  spi_mock_set_interrupt_enabled(SPI_INTERRUPT_TC, false);
#define SPI_SET_MODE(_cpol,_cpha)   spi_mock_set_mode(_cpol, _cpha)
#define SPI_SET_CLOCK_RATE_DIV_4()  spi_mock_set_clock_rate_div(4)
#define SPI_ENABLE()                spi_mock_set_enabled(true)


#define SPI_SET_DATA_REG(val)   spi_mock_write_data_reg(val)
#define SPI_GET_DATA_REG()      spi_mock_read_data_reg()
//#define SET_SPI_STATUS_REG(x) spi_mock_write_status_reg(x)
//#define GET_SPI_STATUS_REG    spi_mock_read_status_reg()

// TODO: implement
#define SPI_CLEAR_FLAGS()	   
#define IS_SPI_INTERRUPT_FLAG_SET() (true)
#define IS_SPI_WRITE_COLLISION_FLAG_SET() (true)

#define SPI_TRANSFER_COMPLETE_VECT  void spi_transfer_complete_vect(void)

#endif
