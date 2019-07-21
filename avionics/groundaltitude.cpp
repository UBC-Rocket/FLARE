/*
 * Ground Altitude Calculation Source
 *
 * @file    groundaltitude.cpp
 * @author  UBC Rocket Avionics 2018/2019
 * @description   Rolling average of barometer data to calculate and set ground
 *      altitude. The three functions in this file initialize the ground
 *      altitude array, returns the sum of that array, and updates the array.
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
#include "options.h"
#include "groundaltitude.h"

#include <Arduino.h>

/*Functions------------------------------------------------------------*/

/* float groundAlt_arr_sum(float ground_alt_arr[]){}
 * @brief  Sums the ground altitude array.
 * @param  ground_alt_arr - ground altitude array, of size ground_alt_size.
 * @return float sum - The sum of the global ground altitude array.
 */
float groundAlt_arr_sum(float ground_alt_arr[]){
    float sum = 0;
    int j;
    for (j = 0; j < GROUND_ALT_SIZE; j++){
        sum += ground_alt_arr[j];
    }
    return sum;
}

/* float groundAlt_update(float *bar_data, float ground_alt_arr[]){}
 * @brief  Updates the ground altitude array with new barometer data, applies a
 *      moving average to the values, and averages the array with the new
 *      values.
 * @param  *bar_data - Sensor data from the barometer in millibars.
 * @param  float ground_alt_arr[] - array to store ground altitude information.
 * @return float baseline_pressure_update - The updated moving average of the
 *          ground altitude array, in millibars.
 */
float groundAlt_update(float *bar_data, float ground_alt_arr[]){
    static float prev_bar_data = ground_alt_arr[GROUND_ALT_SIZE-1];
    static int k = 0;
    float sum_arr_update, baseline_pressure_update;

    if (k >= GROUND_ALT_SIZE){
        k = 0;
    }
    ground_alt_arr[k] = prev_bar_data;
    prev_bar_data = *bar_data;
    sum_arr_update = groundAlt_arr_sum(ground_alt_arr);
    baseline_pressure_update = (sum_arr_update / GROUND_ALT_SIZE);
    k++;

    #ifdef TESTING
    SerialUSB.print("GROUND PRESSURE UPDATE: ");
    SerialUSB.println(baseline_pressure_update);
    #endif

    return baseline_pressure_update;
}
