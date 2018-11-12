/*Ground Altitude Calculation Source*/

/*Includes------------------------------------------------------------*/
#include "sensors.h"
#include "groundaltitude.h"

#include <Arduino.h>

/*Variables------------------------------------------------------------*/
float ground_alt_arr[ground_alt_size];
//float prev_bar_data;

/*Functions------------------------------------------------------------*/

float groundAlt_init(float *barometer_data_init){
    int i = 0;
    float baseline_pressure_init, sum_arr;
    for (i = 0; i < ground_alt_size; i ++){
        ground_alt_arr[i] = *barometer_data_init;
    }
    sum_arr = groundAlt_arr_sum();
//  prev_bar_data = *barometer_data_init;
    baseline_pressure_init = (sum_arr / ground_alt_size);
    return baseline_pressure_init;
}

float groundAlt_arr_sum(){
    float sum = 0;
    int j;
    for (j = 0; j < ground_alt_size; j++){
        sum += ground_alt_arr[j];
    }
    return sum;
}

float groundAlt_update(float *bar_data){
    static float prev_bar_data = ground_alt_arr[ground_alt_size];  // Can you even do this??
    // otherwise comment in code above to make it a global var if not possible
    static int k = 0;
    float sum_arr_update, baseline_pressure_update;

    if (k >= ground_alt_size){
        k = 0;
    }
    ground_alt_arr[k] = prev_bar_data;
    prev_bar_data = *bar_data;
    sum_arr_update = groundAlt_arr_sum();
    baseline_pressure_update = (sum_arr_update / ground_alt_size);
    k++;
    return baseline_pressure_update;
}