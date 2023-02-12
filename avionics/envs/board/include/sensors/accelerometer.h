#pragma once

/**
 * Accelerometer Sensor Class
 */

/*Includes------------------------------------------------------------*/
#include "SparkFun_LIS331.h" // Accelerometer
#include "sensors_interface.h"

/*Constants------------------------------------------------------------*/
// #define ACCELEROMETER_STATUS_POSITION 2
// #define ACCELEROMETER_DATA_ARRAY_SIZE 3

class Accelerometer : public SensorBase<ACCELEROMETER_DATA_LENGTH> {
  public:
    Accelerometer(float *const buf);
    void readData();

  private:
    constexpr static uint8_t ACCELEROMETER_ADDRESS = 0x19;
    constexpr static uint8_t ACCELEROMETER_SCALE = 6;
    LIS331 accelerometer;
    int16_t x, y, z;
};
