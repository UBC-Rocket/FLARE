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

/*Constants------------------------------------------------------------*/

#define MILLISECONDS            1000

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
 * @param  float *pressure_set - Set of pressure values.
 * @return void.
 */
void calculateValues(float acc_data[], float bar_data[],
                    float* prev_altitude, float* altitude, float* delta_altitude,
                    float* prev_delta_altitude, float* baseline_pressure, unsigned long *delta_time, float *pressure_set)
{
    static float average_pressure;
    addToPressureSet(pressure_set, bar_data[0]);
    average_pressure = calculatePressureAverage(pressure_set);

    *prev_altitude = *altitude;
    *prev_delta_altitude = *delta_altitude;
    *altitude = 44330.0 * (1 - powf(average_pressure / *baseline_pressure, 1 / 5.255));
    *delta_altitude = (*altitude - *prev_altitude) * MILLISECONDS / *delta_time;
}

/*
* @brief replaces oldest value in the average pressure set with a new piece of pressure data. All values
*       in the set are default initialized to zero (as per c++ standard), so run the initialization function
*       during startup
* @param float* average_set - the working array of pressure data used to calculate the average pressure.
*       This is mutated by replacing the oldest data as new readings come in.
* @param float data - the new data to add to the set
* @return void
*/
void addToPressureSet(float* average_set, float data){
    static int i = 0;
    average_set[i] = data;
    if(i >= PRESSURE_AVG_SET_SIZE - 1)
        i = 0;
    else
        i++;
}


/*
* @brief Calculates average pressure
* @param float* average_set - the array of pressure data actively being used
* @return float - the average of average set
*/
float calculatePressureAverage(float* average_set){
    float sum = 0;
    for(int i = 0; i < PRESSURE_AVG_SET_SIZE; i++){
        sum = sum + average_set[i];
    }
    return sum / PRESSURE_AVG_SET_SIZE;
}
