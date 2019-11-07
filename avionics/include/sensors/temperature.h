#ifndef TEMPERATURE_H
#define TEMPERATURE_H

/**
  * Temperature Sensor Class
  */

/*Includes------------------------------------------------------------*/
#include "sensors-interface.h"
#include "SparkFunTMP102.h"              // Temperature

/*Constants------------------------------------------------------------*/
#define TEMP_SENSOR_ADDRESS         0x48
#define TEMPERATURE_STATUS_POSITION 4
#define TEMPERATURE_DATA_ARRAY_SIZE 1

class Temperature : public ISensor {
public:
SensorStatus initSensor();
SensorStatus readData(float* data);
uint8_t dataLength();

private:
TMP102* temp_sensor;
float temp;
};

#endif