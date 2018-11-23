/* @file    calculations.cpp
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


/*Includes------------------------------------------------------------*/
#include "calculations.h"
#include <math.h>
#include <Arduino.h>

/*Variables------------------------------------------------------------*/

/*Functions------------------------------------------------------------*/

/* void calculateValues(float [], float[], float*, float*, float*, float*, float*, unsigned long*){}
 * @brief  Calculates current values
 * @param  float acc_data[] - the accelerometer sensor value array
 * @param  float bar_data[] - received barometer sensor data array 
 * @param  float *prev_altitude - previous altitude in meters 
 * @param  float *altitude - Current altitude in meters
 * @param  float *delta_altitude - Change in altitude from the current altitude update in meters/second
 * @param  float *prev_delta_altitude - Previous change in altitude from the altitude update in meters/second 
 * @param  float *baseline_pressure - the baseline pressure of the rocket (calculated ground altitude) in millibars
 * @param  unsigned long *delta_time - time between data polling 
 * @return void.
 */
void calculateValues(float acc_data[], float bar_data[],
                    float* prev_altitude, float* altitude, float* delta_altitude, float*prev_delta_altitude, float* baseline_pressure, unsigned long *delta_time)
{
    //*abs_accel = sqrtf(powf(acc_data[0], 2) + powf(acc_data[1], 2) + powf(acc_data[2]), 2);
    *prev_altitude = *altitude;
    *prev_delta_altitude = *delta_altitude;
    *altitude = 44330.0 * (1 - powf(bar_data[0] / *baseline_pressure, 1 / 5.255));
    *delta_altitude = (altitude - prev_altitude) * MILLISECONDS / *delta_time;
}