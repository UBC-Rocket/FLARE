/* @file    groundaltitude.cpp
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

/*Includes------------------------------------------------------------*/
#include "sensors.h"
#include "groundaltitude.h"

#include <Arduino.h>

/*Variables------------------------------------------------------------*/
float GLOBAL_ground_alt_arr[ground_alt_size];
//float prev_bar_data;

/*Functions------------------------------------------------------------*/
/* float groundAlt_init(float*){}
 * @brief  Initializes the ground altitude array.
 * @param  float *barometer_data_init - Sensor data from the barometer in millibars, read at startup.
 * @return float baseline_pressure_init - The moving average of the now initialized ground altitude array, in millibars.
 */
float groundAlt_init(float *barometer_data_init){
    int i = 0;
    float baseline_pressure_init, sum_arr;
    for (i = 0; i < ground_alt_size; i ++){
        GLOBAL_ground_alt_arr[i] = *barometer_data_init;
    }
    sum_arr = groundAlt_arr_sum();
//  prev_bar_data = *barometer_data_init;
    baseline_pressure_init = (sum_arr / ground_alt_size);
    return baseline_pressure_init;
}

/* float groundAlt_arr_sum(){}
 * @brief  Sums the ground altitude array.
 * @param  GLOBAL_ground_alt_arr - Global ground altitude array, of size ground_alt_size.
 * @return float sum - The sum of the global ground altitude array.
 */
float groundAlt_arr_sum(){
    float sum = 0;
    int j;
    for (j = 0; j < ground_alt_size; j++){
        sum += GLOBAL_ground_alt_arr[j];
    }
    return sum;
}

/* float groundAlt_update(float *bar_data){}
 * @brief  Updates the ground altitude array with new barometer data, applies a weighted moving average
 *      to the values, and sums the array with the new values.
 * @param  *bar_data - Sensor data from the barometer in millibars.
 * @return float baseline_pressure_update - The updated moving average of the ground altitude array, in millibars.
 */
float groundAlt_update(float *bar_data){
    static float prev_bar_data = GLOBAL_ground_alt_arr[ground_alt_size-1];  // Can you even do this??
    // otherwise comment in code above to make it a global var if not possible
    static int k = 0;
    float sum_arr_update, baseline_pressure_update;

    if (k >= ground_alt_size){
        k = 0;
    }
    GLOBAL_ground_alt_arr[k] = prev_bar_data;
    prev_bar_data = *bar_data;
    sum_arr_update = groundAlt_arr_sum();
    baseline_pressure_update = (sum_arr_update / ground_alt_size);
    k++;
    return baseline_pressure_update;
}
