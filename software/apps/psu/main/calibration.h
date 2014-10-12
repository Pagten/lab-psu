/*
 * calibration.h
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

#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @file calibration.h
 * @author Pieter Agten (pieter.agten@gmail.com)
 * @date 12 Oct 2014
 * 
 * This file provides a calibration procedure for mapping ADC values to 
 * voltages and currents and for mapping voltages and currents back to DAC
 * values. The mappings resulting from calibration can be stored in EEPROM and
 * be restored later on.
 */


/**
 * Load the factory default calibration data.
 */
void cal_load_defaults(void);


/**
 * Load the calibration data from the EEPROM.
 * 
 * The EEPROM data has a CRC checksum associated with it. This function will
 * automatically check this checksum while loading the data and return true if
 * the checksum is correct, and false otherwise.
 *
 * @return true if the calibration data was loaded successfully and the CRC
 *         checksum is correct, false otherwise.
 */
bool cal_load_from_eeprom(void);


/**
 * Verify the CRC checksum associated with the calibration data in the
 * EEPROM. This function only verifies the checksum and does not load the
 * data into the corresponding data structures in memory.
 *
 * @return true if the CRC checksum is correct, and false otherwise.
 */
bool cal_verify_eeprom(void);


/**
 * Save the current calibration data to the EEPROM.
 *
 * A CRC checksum is included with the data, to verify the data's integrity
 * when restoring it from the EEPROM. This function will take a long time to
 * execute, on the order of  (ADC_TO_VOLTAGE_NODES + ADC_TO_CURRENT_NODES + 
 * DAC_TO_VOLTAGE_NODES + DAC_TO_CURRENT_NODES) times 7ms.
 */
void cal_save_to_eeprom(void);


/**
 * Convert an ADC voltage measurement value to the corresponding voltage (in
 * millivolts), based on the current calibration data.
 *
 * @param adc The ADC voltage measurement to convert.
 * @return The voltage (in millivolts) corresponding to the given ADC value.
 */
int16_t cal_adc_to_mvolt(uint16_t adc);


/**
 * Convert an ADC current measurement value to the corresponding amperage (in
 * milliamps), based on the current calibration data.
 *
 * @param adc The ADC current measurement to convert.
 * @return The current (in milliamps) corresponding to the given ADC value.
 */
int16_t cal_adc_to_mamp(uint16_t adc);


/**
 * Convert a voltage to the corresponding DAC value, based on the current
 * calibration data.
 *
 * @param mvolts The voltage (in millivolts) to convert.
 * @return The DAC value that corresponds to the given voltage.
 */
uint16_t cal_mvolt_to_dac(uint16_t mvolt);


/**
 * Convert an amperage to the corresponding DAC value, based on the current
 * calibration data.
 *
 * @param mamps The amperage (in milliamps) to convert.
 * @return The DAC value that corresponds to the given amperage.
 */
uint16_t cal_mamp_to_dac(uint16_t mamp);


#endif
