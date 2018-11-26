/*Ground Altitude Calculation Source*/

/*Includes------------------------------------------------------------*/
#include "sensors.h"
#include "groundaltitude.h"
#include "SparkFun_MS5803_I2C.h"    //barometer

#include <Arduino.h>

/*Variables------------------------------------------------------------*/
// MS5803 barometer(ADDRESS_HIGH);

/*Functions------------------------------------------------------------*/
// ask about baseline pressure
//
// handling arrays passing back and forth
// barometer precision
// zero weighting most recent data value
void groundAlt_init(float ground_alt_arr[], float *ground_alt_sum, float *ground_alt_prev, float *ground_alt){
    // int i_alt_init;
    // float barometer_data_init, altitude_init;

    // for (i_alt_init = 0; i_alt_init < ground_alt_size; i_alt_init ++){
    //     barometer_data_init = barometer.getPressure(ADC_4096);
    //     altitude_init = 44330.0 * (1 - powf(barometer_data_init / BASELINE_PRESSURE, 1 / 5.255));
    //     *ground_alt_sum += altitude_init;  //calculated altitude
    //     ground_alt_arr[i_alt_init] = altitude_init;
    //     delay(ground_alt_delay);
    // }
    // barometer_data_init = barometer.getPressure(ADC_4096);
    // *ground_alt_prev = 44330.0 * (1 - powf(barometer_data_init / BASELINE_PRESSURE, 1 / 5.255));
    // *ground_alt = (*ground_alt_sum / ground_alt_size);
    // delay(ground_alt_delay);
}

void groundAlt_update(float ground_alt_arr[], float *ground_alt_sum, float *ground_alt_prev, float *ground_alt, float *current_altitude){
    static int i_alt_update = 0;

    if (i_alt_update >= ground_alt_size){
        i_alt_update = 0;
    }
    *ground_alt_sum -= ground_alt_arr[i_alt_update];
    *ground_alt_sum += *ground_alt_prev;
    ground_alt_arr[i_alt_update] = *ground_alt_prev;
    *ground_alt_prev = *current_altitude;
    *ground_alt = (*ground_alt_sum / ground_alt_size);
    i_alt_update ++;
}
