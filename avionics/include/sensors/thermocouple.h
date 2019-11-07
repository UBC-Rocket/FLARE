#ifndef THERMOCOUPLE_H
#define THERMOCOUPLE_H

/**
  * Thermocouple Sensor Class
  */

/*Includes------------------------------------------------------------*/
#include "sensors-interface.h"

/*Constants------------------------------------------------------------*/
#define THERMOCOUPLE_STATUS_POSITION 8
#define THERMOCOUPLE_DATA_ARRAY_SIZE 1

class Thermocouple : public ISensor {
public:
SensorStatus initSensor();
SensorStatus readData(float* data);
uint8_t dataLength();

private:

};

#endif