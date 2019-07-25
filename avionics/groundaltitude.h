/*
 * Ground Altitude Calculation Header
 *
 * @file    groundaltitude.h
 * @author  UBC Rocket Avionics 2018/2019
 * @description   Rolling average of barometer data to calculate and set ground altitude.
 * The three functions in this file initialize the ground altitude array,
 * returns the sum of that array, and updates the array.
 *
 * @section LICENSE
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * Distributed as-is; no warranty is given.
 */

#ifndef GROUNDALTITUDE_H
#define GROUNDALTITUDE_H

/*Includes------------------------------------------------------------*/
#include <stdint.h>

/*Constants------------------------------------------------------------*/
#define GROUND_ALT_SIZE 60  //array size for moving average

/*Functions------------------------------------------------------------*/
float groundAlt_arr_sum(float ground_alt_arr[]);
float groundAlt_update(float *bar_data, float ground_alt_arr[]);

#endif
