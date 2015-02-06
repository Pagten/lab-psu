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

typedef enum {
  CAL_PROCESS_VOLTAGE_ADC,
  CAL_PROCESS_CURRENT_ADC,
  CAL_PROCESS_VOLTAGE_DAC,
  CAL_PROCESS_CURRENT_DAC,
} cal_process_type;

typedef enum {
  CAL_PROCESS_OK,
  CAL_PROCESS_ALREADY_RUNNING,
  CAL_PROCESS_INVALID_STATE,
  CAL_PROCESS_INVALID_VALUE,
} cal_process_status;

typedef enum {
  CAL_PROCESS_IDLE,
  CAL_PROCESS_RUNNING,
  CAL_PROCESS_ERROR_OVERVOLTAGE,
  CAL_PROCESS_ERROR_UNDERVOLTAGE,
  CAL_PROCESS_ERROR_OVERCURRENT,
  CAL_PROCESS_ERROR_UNDERCURRENT,
} cal_process_state;

typedef struct {
  cal_process_type type;
  cal_process_state state;
  uint8_t step;
} cal_process;

/**
 * Initialize a calibration process structure. This function
 * should only be called once for any calibration process 
 * structure.
 *
 * @param p The calibration process structure to initialize.
 */
void cal_process_init(cal_process* p);

/**
 * Start a new calibration process.
 *
 * @param p    The calibration process structure to use.
 * @param type The type of calibration process to start.
 * @return CAL_PROCESS_OK if the calibration process was started successfuly,
 *         or CAL_PROCESS_ALREADY_RUNNING if another calibration process is
 *         already running.
 */
cal_process_status
cal_process_start(cal_process* p, cal_process_type type);

/**
 * Move to the next calibration point in a given ADC calibration process.
 *
 * @param p   The ADC calibration process in which to move to the next point.
 * @param val The output value in the current step.
 * @result CAL_PROCESS_OK if the given value is valid and the calibration 
 *         process has been moved to the next step, CAL_PROCESS_INVALID_STATE
 *         if the process is not in a valid state to move to the next step, or
 *         CAL_PROCESS_INVALID_VALUE if the given value is invalid.
 */
cal_process_status
cal_process_adc_next(cal_process* p, uint16_t val);

/**
 * Cancel an ongoing calibration process.
 *
 * @param p The calibration process to cancel.
 * @result CAL_PROCESS_OK if the process was cancelled successfully, or 
 *         CAL_PROCESS_INVALID_STATE if the process is not in a valid state to
 *         be cancelled.
 */
cal_process_status
cal_process_cancel(cal_process* p);

/**
 * Commit a finished calibration process. After calling this function, the
 * calibration process' values will be used when converting ADC/DAC readings,
 * but they will not be saved to EEPROM (for this, see cal_save_to_eeprom()).
 *
 * @param p The calibration process to commit.
 * @result CAL_PROCESS_OK if the process' values were committed successfully,
 *         or CAL_PROCESS_INVALID_STATE if the given process is not in a valid
 *         state to be committed.
 */
cal_process_status
cal_process_commit(cal_process* p);

/**
 * Return the current state of a given calibration process.
 *
 * @param p The calibration process of which to return the current state.
 * @result The current state of the given calibration process.
 */
cal_process_state
cal_process_get_state(cal_process* p);

/**
 * Return the number of the step that a given calibration process is in.
 *
 * @param p The calibration process for which to get the current step number.
 * @result The number of the step the given process is in.
 */
uint8_t cal_process_get_step_number(cal_process* p);

/**
 * Return the output value of the step that a given calibration process is in.
 *
 * @param p The calibration process for which to get the current step value.
 * @result The output value of the step the given process is in.
 */
uint16_t cal_process_get_step_value(cal_process* p);

/**
 * Return whether there is some calibration process running.
 *
 * @result True if any calibration process is running, or false otherwise.
 */
bool cal_is_process_running();


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
