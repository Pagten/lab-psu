/*
 * devices.h
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

#ifndef DEVICES_H
#define DEVICES_H

#if defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__)

// Pin to PCINT mapping
#define PCINT_B0 PCINT0
#define PCINT_B1 PCINT1
#define PCINT_B2 PCINT2
#define PCINT_B3 PCINT3
#define PCINT_B4 PCINT4
#define PCINT_B5 PCINT5
#define PCINT_B6 PCINT6
#define PCINT_B7 PCINT7
#define PCINT_C0 PCINT8
#define PCINT_C1 PCINT9
#define PCINT_C2 PCINT10
#define PCINT_C3 PCINT11
#define PCINT_C4 PCINT12
#define PCINT_C5 PCINT13
#define PCINT_C6 PCINT14
// NO C7
#define PCINT_D0 PCINT16
#define PCINT_D1 PCINT17
#define PCINT_D2 PCINT18
#define PCINT_D3 PCINT19
#define PCINT_D4 PCINT20
#define PCINT_D5 PCINT21
#define PCINT_D6 PCINT22
#define PCINT_D7 PCINT23


// Pin to PCINT_vect mapping
#define PCINT_vect_B0 PCINT0_vect
#define PCINT_vect_B1 PCINT0_vect
#define PCINT_vect_B2 PCINT0_vect
#define PCINT_vect_B3 PCINT0_vect
#define PCINT_vect_B4 PCINT0_vect
#define PCINT_vect_B5 PCINT0_vect
#define PCINT_vect_B6 PCINT0_vect
#define PCINT_vect_B7 PCINT0_vect
#define PCINT_vect_C0 PCINT1_vect
#define PCINT_vect_C1 PCINT1_vect
#define PCINT_vect_C2 PCINT1_vect
#define PCINT_vect_C3 PCINT1_vect
#define PCINT_vect_C4 PCINT1_vect
#define PCINT_vect_C5 PCINT1_vect
#define PCINT_vect_C6 PCINT1_vect
// NO C7
#define PCINT_vect_D0 PCINT2_vect
#define PCINT_vect_D1 PCINT2_vect
#define PCINT_vect_D2 PCINT2_vect
#define PCINT_vect_D3 PCINT2_vect
#define PCINT_vect_D4 PCINT2_vect
#define PCINT_vect_D5 PCINT2_vect
#define PCINT_vect_D6 PCINT2_vect
#define PCINT_vect_D7 PCINT2_vect


// Pin to PCIE mapping
#define PCIE_B0 PCIE0
#define PCIE_B1 PCIE0
#define PCIE_B2 PCIE0
#define PCIE_B3 PCIE0
#define PCIE_B4 PCIE0
#define PCIE_B5 PCIE0
#define PCIE_B6 PCIE0
#define PCIE_B7 PCIE0
#define PCIE_C0 PCIE1
#define PCIE_C1 PCIE1
#define PCIE_C2 PCIE1
#define PCIE_C3 PCIE1
#define PCIE_C4 PCIE1
#define PCIE_C5 PCIE1
#define PCIE_C6 PCIE1
// NO C7
#define PCIE_D0 PCIE2
#define PCIE_D1 PCIE2
#define PCIE_D2 PCIE2
#define PCIE_D3 PCIE2
#define PCIE_D4 PCIE2
#define PCIE_D5 PCIE2
#define PCIE_D6 PCIE2
#define PCIE_D7 PCIE2


// Pin to PCMSK mapping
#define PCMSK_B0 PCMSK0
#define PCMSK_B1 PCMSK0
#define PCMSK_B2 PCMSK0
#define PCMSK_B3 PCMSK0
#define PCMSK_B4 PCMSK0
#define PCMSK_B5 PCMSK0
#define PCMSK_B6 PCMSK0
#define PCMSK_B7 PCMSK0
#define PCMSK_C0 PCMSK1
#define PCMSK_C1 PCMSK1
#define PCMSK_C2 PCMSK1
#define PCMSK_C3 PCMSK1
#define PCMSK_C4 PCMSK1
#define PCMSK_C5 PCMSK1
#define PCMSK_C6 PCMSK1
// NO C7
#define PCMSK_D0 PCMSK2
#define PCMSK_D1 PCMSK2
#define PCMSK_D2 PCMSK2
#define PCMSK_D3 PCMSK2
#define PCMSK_D4 PCMSK2
#define PCMSK_D5 PCMSK2
#define PCMSK_D6 PCMSK2
#define PCMSK_D7 PCMSK2

#endif

#endif
