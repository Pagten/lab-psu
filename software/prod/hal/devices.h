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
#define PCINT_B0 0
#define PCINT_B1 1
#define PCINT_B2 2
#define PCINT_B3 3
#define PCINT_B4 4
#define PCINT_B5 5
#define PCINT_B6 6
#define PCINT_B7 7
#define PCINT_C0 8
#define PCINT_C1 9
#define PCINT_C2 10
#define PCINT_C3 11
#define PCINT_C4 12
#define PCINT_C5 13
#define PCINT_C6 14
// NO C7
#define PCINT_D0 16
#define PCINT_D1 17
#define PCINT_D2 18
#define PCINT_D3 19
#define PCINT_D4 20
#define PCINT_D5 21
#define PCINT_D6 22
#define PCINT_D7 23


// PCINT to PCI mapping
#define PCI_PCINT0  0
#define PCI_PCINT1  0
#define PCI_PCINT2  0
#define PCI_PCINT3  0
#define PCI_PCINT4  0
#define PCI_PCINT5  0
#define PCI_PCINT6  0
#define PCI_PCINT7  0
#define PCI_PCINT8  1
#define PCI_PCINT9  1
#define PCI_PCINT10 1
#define PCI_PCINT11 1
#define PCI_PCINT12 1
#define PCI_PCINT13 1
#define PCI_PCINT14 1
// NO PCI15
#define PCI_PCINT16 2
#define PCI_PCINT17 2
#define PCI_PCINT18 2
#define PCI_PCINT19 2
#define PCI_PCINT20 2
#define PCI_PCINT21 2
#define PCI_PCINT22 2
#define PCI_PCINT23 2


#endif

#endif
