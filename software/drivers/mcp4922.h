/*
 * mcp4922.h
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

#ifndef MCP4922_H
#define MCP4922_H


/**
 * @file mcp4922.h
 * @author Pieter Agten (pieter.agten@gmail.com)
 * @date 27 dec 2013
 *
 * This is a driver for the Microchip MCP4922, a dual 12-bit digital-to-analog-
 * converter with SPI interface.
 *
 * Not all features are supported, in particular:
 * * BUF is always set to 0, hence the input buffer amplifier is disabled
 * * ~GA is always set to 1, hence the output voltage doubler is disabled
 * * ~SHDN is always set to 1, hence the output buffer cannot be shut down
 */

#include <stdint.h>

typedef enum {
  MCP4922_CHANNEL_A,
  MCP4922_CHANNEL_B
} mcp4922_channel;

typedef enum {
  MCP4922_OK,
  MCP4922_ERROR
} mcp4922_status;


/**
 * The type of the callback function called after the DAC value has been set.
 * 
 * @arg status  Indicates whether setting the DAC value was succesful
 * @arg data    Opaque data pointer given to mcp4922_set().
 */
typedef void (*mcp4922_set_callback)(mcp4922_status status, void *data);


/**
 * Initialize the MCP4922 driver.
 *
 * Modules that must be initialized first:
 *  spi_master
 */
void mcp4922_init(void);


/**
 * Set the output value of one of the DAC's output channels
 *
 * @arg cs_port  The slave select port to which the DAC is connected
 * @arg cs_pin   The pin of cs_port to which the DAC is connected
 * @arg ch_value Indicates the channel to set and the value to set it to. If
 *               the MSB is 0, channel A is selected, otherwise channel B is 
 *               selected. The lowest 12 bits indicate the value to set.
 * @arg cb       Callback function that will be called after the DAC value has
 *               been set, or if an error occurs during transmission
 * @arg cb_data  Opaque pointer that will be passed on to the cb callback
 * @return MCP4922_OK if the SPI data transfer to set the output value was
 *         scheduled succesfully, MCP4922_ERROR otherwise.
 */
mcp4922_status mcp4922_set(volatile uint8_t *cs_port, uint8_t cs_pin,
			   uint16_t ch_value, mcp4922_set_callback cb, 
			   void* cb_data);

#endif
