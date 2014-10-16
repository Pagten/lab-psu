/*
 * pwlf.h
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

#ifndef PWLF_H
#define PWLF_H

/**
 * @file pwlf.h
 * @author Pieter Agten (pieter.agten@gmail.com)
 * @date 10 Oct 2014
 *
 * This module implements continuous 16-bit to 16-bit integer 16-piece
 * piecewise linear functions.  When given an argument below or above the
 * domain, the function value will be extrapolated based on the two smallest
 * or the two largest nodes respectively.
 */

#include <stdint.h>

typedef struct {
  uint16_t x;
  int16_t y;
} pwlf_pair;

typedef struct {
  uint8_t count;
  uint8_t max_count;
  pwlf_pair values[];
} pwlf;


typedef enum {
  PWLF_ADD_NODE_OK,
  PWLF_ADD_NODE_FULL,
  PWLF_ADD_NODE_INVALID_X,
} pwlf_add_node_status;

typedef enum {
  PWLF_REMOVE_NODE_OK,
  PWLF_REMOVE_NODE_EMPTY,
} pwlf_remove_node_status;



#define PWLF_INIT(SIZE)						\
  { .count = 0, .max_count = SIZE, .values = { [SIZE-1] = {0} } }


/**
 * Initialize a piecewise linear function structure.
 *
 * The function will initialy be 0 over its entire domain.
 *
 * @param f     The piecewise linear function to initialize
 * @param buf   A buffer of at least size * sizeof(pwlf_pair) bytes
 * @param size  The maximum number of nodes the function can have
 */
void pwlf_init(pwlf* f, pwlf_pair buf[], uint8_t size);


/**
 * Clear all nodes from a piecewise linear function.
 *
 * The function will be 0 over its entire domain.
 */
void pwlf_clear(pwlf* f);


/**
 * Return the number of nodes of a piecewise linear function.
 *
 * @param f The piecewise linear function of which to return the number of
 *          nodes
 * @return The number of nodes of the given function f.
 */
uint8_t pwlf_get_count(pwlf* f);


/**
 * Return the size of a piecewise linear function's node buffer.
 *
 * @param f The piecewise linear function of which to return the node buffer
 *        size
 * @return The size of the node buffer assigned to f on initialization.
 */
uint8_t pwlf_get_size(pwlf* f);


/**
 * Return the x value corresponding to the ith node of a function.
 *
 * @param f  The piecewise linear function of which to return the ith node
 *           x value.
 * @param i  The number of the node of which to return the x value. Must be
 *           between 0 and pwlf_get_count(f).
 * @return The x value of the ith node of f, or 0 if i is out of range.
 */
uint16_t pwlf_get_x(pwlf* f, uint8_t i);


/**
 * Return the y value corresponding to the ith node of a function.
 *
 * @param f  The piecewise linear function of which to return the ith node
 *           y value.
 * @param i  The number of the node of which to return the y value. Must be
 *           between 0 and pwlf_get_count(f).
 * @return The y value of the ith node of f, or 0 if i is out of range.
 */
int16_t pwlf_get_y(pwlf* f, uint8_t i);


/**
 * Add a node to a piecewise linear function.
 * 
 * @param f The piecewise linear function to which to add a node
 * @param x The x value of the node to add
 * @param y The y value of the node to add
 * @return PWLF_ADD_NODE_OK if the node was added successfully,
 *         PWLF_ADD_NODE_FULL if the function's node buffer is already full,
 *         or PWLF_ADD_NODE_INVALID if the given x value is smaller than or
 *         equal to the last added node's x value (if any).
 */
pwlf_add_node_status
pwlf_add_node(pwlf* f, uint16_t x, int16_t y);


/**
 * Remove the last added node from a piecewise linear function.
 * 
 * @param f The piecewise linear function to which to remove the last node
 * @return PWLF_REMOVE_NODE_OK if the last added node was removed
 *         successfully, or PWLF_REMOVE_NODE_EMPTY if the function doesn't
 *         contain any nodes.
 */
pwlf_remove_node_status
pwlf_remove_node(pwlf* f);


/**
 * Return the y value of a piecewise linear function at a given x value.
 * 
 * @param x The x value at which to get the function value.
 * @return The function value at the given x value.
 */
int16_t pwlf_value(pwlf* f, uint16_t x);


#endif
