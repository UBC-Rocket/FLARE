#pragma once

/**
 * Thermocouple Sensor Class
 */

/*Includes------------------------------------------------------------*/
#include "sensors_interface.h"

/*Constants------------------------------------------------------------*/

class Thermocouple : public ISensor {
  public:
    void initSensor() {}
    void readData() {}
    uint8_t dataLength() { return THERMOCOUPLE_DATA_LENGTH; }
    float *getData() { return data; }
    SensorStatus getStatus() { return SensorStatus::NOMINAL; }

  private:
    float data[THERMOCOUPLE_DATA_LENGTH] = {
        200}; // thermocouples are in hot places
};
