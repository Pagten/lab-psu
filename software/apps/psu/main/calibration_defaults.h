/*
 * calibration_defaults.h
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

#ifndef CALIBRATION_DEFAULTS_H
#define CALIBRATION_DEFAULTS_H

/**
 * @file calibration_defaults.h
 * @author Pieter Agten (pieter.agten@gmail.com)
 * @date 12 Oct 2014
 *
 * This file contains the default ADC and DAC calibration values.
 */


#define _S(v) pwlf_itou(v)

// ************** ADCs **************
#define ADC_MIN  0
#define ADC_MAX  UINT16_MAX

// ADC to voltage
#define ADC_TO_MVOLT_MIN     ADC_MIN, _S(16000)
#define ADC_TO_MVOLT_MAX     ADC_MAX, _S(-500)

// ADC to current
#define ADC_TO_MAMP_MIN      ADC_MIN, _S(0)
#define ADC_TO_MAMP_MAX      ADC_MAX, _S(3400)
// ***********************************


// ************** DACs **************
#define DAC_MIN 0
#define DAC_MAX ((1 << 12) - 1)

// Voltage to DAC
#define MVOLT_TO_DAC_MIN  0,     DAC_MIN
#define MVOLT_TO_DAC_MAX  16000, DAC_MAX

// Current to DAC
#define MAMP_TO_DAC_MIN   0,    DAC_MIN
#define MAMP_TO_DAC_MAX   3300, DAC_MAX
// ***********************************

#endif
