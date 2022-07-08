#pragma once

/**
 * Accelerometer Sensor Class
 */

/*Includes------------------------------------------------------------*/
#include "SparkFun_LIS331.h" // Accelerometer
#include "sensors_interface.h"
#include "Wire.h" // for tantalus lite version

/*Constants------------------------------------------------------------*/
// #define ACCELEROMETER_STATUS_POSITION 2
// #define ACCELEROMETER_DATA_ARRAY_SIZE 3

class Accelerometer : public SensorBase<ACCELEROMETER_DATA_LENGTH> {
  public:
    Accelerometer(float *const buf);
    void readData();

  private:
    // modified for tantalus lite version
    // constexpr static uint8_t ACCELEROMETER_ADDRESS = 0x18;
    // constexpr static uint8_t ACCELEROMETER_SCALE = 24;
    // LIS331 accelerometer;
    int16_t x, y, z;
    void getRawAccel(int16_t* ax, int16_t* ay, int16_t* az);
    int Accelerometer::twosComplementToInt(int16_t input);
};
