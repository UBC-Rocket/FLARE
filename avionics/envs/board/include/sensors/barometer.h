#pragma once
/**
 * Barometer Sensor Class
 */

/*Includes------------------------------------------------------------*/
#include "MS5803_01.h" // Barometer
#include "sensors_interface.h"

/*Constants------------------------------------------------------------*/
// #define BAROMETER_STATUS_POSITION 3
// #define BAROMETER_DATA_ARRAY_SIZE 2

class Barometer : public SensorBase<BAROMETER_DATA_LENGTH> {
  public:
    Barometer(float *const buf);
    void readData();

  private:
    MS_5803 barometer;
};
