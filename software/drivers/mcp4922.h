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
 * - BUF is always set to 0, hence the input buffer amplifier is disabled
 * - ~GA is always set to 1, hence the output voltage doubler is disabled
 * - ~SHDN is always set to 1, hence the output buffer cannot be shut down
 */

#include <stdbool.h>
#include <stdint.h>

typedef enum {
  MCP4922_CHANNEL_A,
  MCP4922_CHANNEL_B,
} mcp4922_channel;

typedef enum {
  MCP4922_QUEUE_OK,
  MCP4922_QUEUE_ERROR,
} mcp4922_queue_status;


/**
 * The MCP4922 packet data structure.
 */
typedef struct {
  uint8_t data[2];
  spim_trx spim_trx;
} mcp4922_pkt;



/**
 * Initialize the MCP4922 driver.
 *
 * Modules that must be initialized first:
 *  spi_master
 */
void mcp4922_init(void);


/**
 * Initialize an MCP4922 packet data structure.
 *
 * This function should not be called on a packet that is in transmission.
 * 
 * @param pkt    The MCP4922 packet data structure to initialize
 * @param pin    The number of the pin connected to the MCP4922's CS pin
 * @param port   The port of the pin connected to the MCP4922's CS pin
 * @param ch     The output channel to set
 * @param value  The output value for the channel (only the 12 LSB's are used)
 */
void
mcp4922_pkt_init(mcp4922_pkt* pkt, uint8_t pin, volatile uint8_t* port,
		 mcp4922_channel ch, uint16_t value);


/**
 * Return whether the MCP4922 packet is in transmission.
 * 
 * @param pkt  The MCP4922 packet for which to get the transfer status
 * @return true if the packet is in transmission, false otherwise.
 */
bool mcp4922_pkt_is_in_transmission(mcp4922_pkt* pkt);


/**
 * Queue an MCP4922 packet for transmission.
 *
 * The packet will be transmitted as soon as all previously queued SPI
 * transfers have finished and the SPI hardware becomes free. The transfer
 * must first be initialized using the mcp4922_pkt_init() function.
 *
 * @param pkt  The MCP4922 packet to queue
 * @return MCP4922_QUEUE_OK if the packet was queued succesfully, or
 *         MCP4922_QUEUE_ERROR if the transfer was not initialized using the
 *         mcp4922_pkt_init() function before queing it, or if was already
 *         queued.
 */
mcp4922_queue_status mcp4922_queue(mcp4922_pkt* pkt);



#endif
