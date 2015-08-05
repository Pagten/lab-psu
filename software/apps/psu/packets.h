/*
 * packets.h
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

#ifndef PACKETS_H
#define PACKETS_H

/**
 * @file packets.h
 * @author Pieter Agten (pieter.agten@gmail.com)
 * @date 27 Sep 2014
 *
 * This file defines the structure of SPI packets exchanged by the main and
 * IO-panel MCUs.
 */

#include <stdint.h>

#define IOPANEL_REQUEST_TYPE       0x01
#define IOPANEL_RESPONSE_TYPE      0x81


struct iopanel_request_normal {
  uint8_t mode_flags;
  uint16_t set_voltage;
  uint16_t set_current;
  uint16_t voltage;
  uint16_t current;
};

struct iopanel_request_calibrating {
  uint8_t mode_flags;
  uint8_t type_step;
  uint16_t value;
};

struct iopanel_request_error {
  uint8_t mode_flags;
  uint8_t error;
};

struct iopanel_request {
  union {
    struct iopanel_request_normal normal;
    struct iopanel_request_calibrating calibrating;
    struct iopanel_request_error error;
  } d;
};

struct iopanel_response_normal {
  uint8_t mode_flags;
  uint16_t set_voltage;
  uint16_t set_current;
};

struct iopanel_response_calibrating {
  uint8_t mode_flags;
  uint8_t type_step;
  uint16_t value;
};

struct iopanel_response_error {
  uint8_t mode_flags;
  uint8_t error_code;
};

struct iopanel_response {
  union {
    struct iopanel_response_normal normal;
    struct iopanel_response_calibrating calibrating;
    struct iopanel_response_error error;
  } d;
};

#endif
