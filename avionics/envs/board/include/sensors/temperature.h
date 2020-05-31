#pragma once
/**
 * Temperature Sensor Class
 */

/*Includes------------------------------------------------------------*/
#include "SparkFunTMP102.h" // Temperature
#include "sensors-interface.h"

/*Constants------------------------------------------------------------*/
// #define TEMPERATURE_STATUS_POSITION 4

class Temperature : public SensorBase<1> {
  public:
    Temperature(float *const data);
    void readData();

  private:
    constexpr static uint8_t TEMP_SENSOR_ADDRESS = 0x48;
    TMP102 temp_sensor;
};
