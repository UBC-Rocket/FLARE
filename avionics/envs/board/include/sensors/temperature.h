#pragma once
/**
 * Temperature Sensor Class
 */

/*Includes------------------------------------------------------------*/
#include "MCP9803.h" // Temperature
#include "sensors_interface.h"

/*Constants------------------------------------------------------------*/
// #define TEMPERATURE_STATUS_POSITION 4

class Temperature : public SensorBase<TEMPERATURE_DATA_LENGTH> {
  public:
    Temperature(float *const data);
    void readData();

  private:
    constexpr static uint8_t TEMP_SENSOR_ADDRESS = 0x48;
};
