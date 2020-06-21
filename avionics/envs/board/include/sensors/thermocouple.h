#pragma once

/**
 * Thermocouple Sensor Class
 */

/*Includes------------------------------------------------------------*/
#include <MAX31855k.h>

#include "sensors_interface.h"

/*Constants------------------------------------------------------------*/
// #define THERMOCOUPLE_STATUS_POSITION 8
// #define THERMOCOUPLE_DATA_ARRAY_SIZE 1

class Thermocouple : public SensorBase<1> {
  public:
    Thermocouple(float *const data);
    void readData();

  private:
    MAX31855k probe;
};
