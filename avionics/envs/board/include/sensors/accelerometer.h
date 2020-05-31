#pragma once

/**
 * Accelerometer Sensor Class
 */

/*Includes------------------------------------------------------------*/
#include "SparkFun_LIS331.h" // Accelerometer
#include "sensors-interface.h"

/*Constants------------------------------------------------------------*/
// #define ACCELEROMETER_STATUS_POSITION 2
// #define ACCELEROMETER_DATA_ARRAY_SIZE 3

class Accelerometer : public SensorBase<3> {
  public:
    Accelerometer(float *const buf);
    void readData();

  private:
    constexpr static uint8_t ACCELEROMETER_ADDRESS = 0x18;
    constexpr static uint8_t ACCELEROMETER_SCALE = 24;
    LIS331 accelerometer;
    int16_t x, y, z;
};
