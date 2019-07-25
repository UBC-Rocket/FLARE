/*
 * Calculations Header
 *
 * @file    calculations.h
 * @author  UBC Rocket Avionics 2018/2019
 * @description   Calculates usable flight data from the raw sensor outputs
 *
 * @section LICENSE
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * Distributed as-is; no warranty is given.
 */

#ifndef CALCULATIONS_H
#define CALCULATIONS_H

/*Includes------------------------------------------------------------*/
#include <stdint.h>
#include "sensors.h"

/*Constants------------------------------------------------------------*/
#define PRESSURE_AVG_SET_SIZE   15

/*Functions------------------------------------------------------------*/
void calculateValues(float bar_data[], float* prev_altitude, float* altitude,
            float* delta_altitude, float* baseline_pressure,
            unsigned long *delta_time, float pressure_set[],
            unsigned long delta_time_set[]);
void addToPressureSet(float* average_set, float data);
float calculatePressureAverage(float* average_set);
void addToDeltaTimeSet(unsigned long* average_set, unsigned long data);
float calculateDeltaTimeAverage(unsigned long* average_set);

#endif
