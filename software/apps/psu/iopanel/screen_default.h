/*
 * screen_default.h
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

#ifndef SCREEN_DEFAULT_H
#define SCREEN_DEFAULT_H

/**
 * @file screen_default.h
 * @author Pieter Agten (pieter.agten@gmail.com)
 * @date 05 Aug 2015
 *
 */

PROCESS(screen_default_process);

// TODO: move to general screen module
#define EVENT_SCREEN_LEFT_ROTARY_TICK   (process_event_t)0x80
#define EVENT_SCREEN_RIGHT_ROTARY_TICK  (process_event_t)0x81
#define EVENT_SCREEN_LEFT_ROTARY_CLICK  (process_event_t)0x82
#define EVENT_SCREEN_RIGHT_ROTARY_CLICK (process_event_t)0x83
#define EVENT_SCREEN_PSU_STATUS_UPDATED (process_event_t)0x84
#define EVENT_SCREEN_REDRAW             (process_event_t)0x85


#endif
