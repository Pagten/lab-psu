/*
 * spi_slave.h
 *
 * Copyright 2014 Pieter Agten
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

#ifndef SPI_SLAVE_H
#define SPI_SLAVE_H

/**
 * @file spi_slave.h
 * @author Pieter Agten <pieter.agten@gmail.com>
 * @date 30 Jun 2014
 *
 * This file provides SPI slave communication using a simple link layer
 * protocol designed for request-response type messages. The protocol allows
 * the slave device to specify the length of its response. It also allows the
 * slave device to delay its response for up to 16 bytes after the master has
 * sent its message, which can be useful if the slave needs some time to
 * generate its response. The detailed description of the protocol can be
 * found in spi_master.h.
 */

#include "core/events.h"
#include "core/process.h"


typedef enum {
  SPIS_SEND_RESPONSE_OK,
  SPIS_SEND_RESPONSE_INVALID_TYPE,
  SPIS_SEND_RESPONSE_PAYLOAD_IS_NULL,
  SPIS_SEND_RESPONSE_NO_TRX_IN_PROGRESS
} spis_send_response_status;


/**
 * Initialize the SPI slave module.
 *
 * Modules that should be initialized first:
 *  - process
 */
void spis_init();

/**
 * Set the process to notify on incoming data.
 *
 * The SPI slave module will send the SPIS_MESSAGE_RECEIVED event to the 
 * process when a message has been successfully received. The process then has
 * some time (15 SPI transmission periods) to calculate a response and register
 * it with the SPI slave module using the spis_send_response() function. When
 * response has been sent successfully, the SPIS_RESPONSE_TRANSMITTED event
 * will be sent to the process. If the master ends the transfer before the
 * response can be delivered completely, the SPIS_RESPONSE_ERROR will be sent
 * to the process. Note that there is no guarantee that the master has success-
 * fully received the response, even when the SPIS_TRX_COMPLETED event is sent.
 *
 * The SPI slave module will ensure the integrity of the received message
 * until the spis_send_response() function is called. Hence, the callback 
 * process must copy any data it needs from the message before making this
 * call. It is important that the spis_send_response() function is called for
 * each received message, even if no payload needs to be sent in response,
 * since otherwise the SPI receive buffer will not be freed (and hence the
 * slave cannot receive any new messages from the master).
 *
 * @param p  The process to notify when a message from the SPI master was
 *           received
 */
void spis_register_callback(process* p);

/**
 * Send a response in reply to a message from the SPI master. This function
 * should be called within 15 SPI clock periods after the process set using
 * the spis_register_callback() function has received the
 * SPIS_MESSAGE_RECEIVED event. The process will be notified with the
 * SPIS_RESPONSE_ERROR event if the master ends the transfer before the
 * response can be delivered completely. The process will be notified with the
 * SPIS_RESPONSE_TRANSMITTED event when the response has been transmitted
 * completely, but this does not guarantee that the master has successfully
 * received the payload. The master can always retransmit the original message
 * if it did not successfully receive the response (master and slave should
 * ensure the commands they implement are idempotent). The caller process
 * should ensure the integrity of the payload buffer until it receives the
 * SPIS_RESPONSE_ERROR or SPIS_RESPONSE_TRANSMITTED events.
 *
 * @param type    The response type id, must be smaller than MAX_RESPONSE_TYPE
 * @param payload The payload to send as response (can be NULL if size is 0)
 * @param size    The size of the payload (in number of bytes)
 * @return SPIS_SEND_RESPONSE_INVALID_TYPE if the given type is larger than
 *         MAX_RESPONSE_TYPE, SPIS_SEND_RESPONSE_PAYLOAD_IS_NULL if the given
 *         payload is NULL but size is greater than 0,
 *         SPIS_SEND_RESPONSE_NO_TRX_IN_PROGRESS if there is no transfer in
 *         progress to which to reply, or SPIS_SEND_RESPONSE_OK otherwise.
 */
spis_send_response_status
spis_send_response(uint8_t type, uint8_t* payload, uint8_t size);


/**
 * Return the size (in bytes) of the last payload received from the SPI master.
 *
 * It is only safe to call this function after the SPIS_MESSAGE_RECEIVED event
 * has been sent to the callback process and before the corresponding call to
 * the spis_send_response() function.
 *
 * @return The size (in bytes) of the last received SPI payload.
 */
uint8_t spis_get_rx_size(void);


/**
 * Return a pointer to the last payload received from the SPI master.
 *
 * It is only safe to call this function after the SPIS_MESSAGE_RECEIVED event
 * has been sent to the callback process and before the corresponding call to
 * the spis_send_response() function.
 *
 * @return A pointer to the last payload received from the SPI master.
 */
uint8_t* spis_get_rx_data(void);


#endif
