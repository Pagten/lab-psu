/*
 * control.h
 *
 * Copyright 2015 Pieter Agten
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

#ifndef CONTROL_H
#define CONTROL_H

/**
 * @file control.h
 * @author Pieter Agten (pieter.agten@gmail.com)
 * @date 22 Jul 2015
 */


typedef enum {
  CTRL_CH_VOLTAGE0,
  CTRL_CH_CURRENT0,
  CTRL_NB_CHANNELS
} ctrl_channel;

/**
 * Initialize the control module.
 *
 * Modules that should be initialized first:
 *  * process
 *  * adc
 *  * mcp4922
 */
void ctrl_init(void)


/**
 * Set the output value of a given channel.
 *
 * @param ch  The channel to set.
 * @param val The value to set the channel to.
 */
void ctrl_set_output(ctrl_channel ch, uint16_t val);

/**
 * Return the current value of a given channel.
 *
 * @param ch The channel of which to return the value.
 * @return The value of the specified channel.
 */
uint16_t ctrl_get_input(ctrl_channel ch);


#endif
