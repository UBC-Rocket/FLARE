#ifndef TEMPERATURE_H
#define TEMPERATURE_H

/**
  * Temperature Sensor Class
  */

/*Includes------------------------------------------------------------*/
#include "sensors-interface.h"
#include "SparkFunTMP102.h" // Temperature

/*Constants------------------------------------------------------------*/
#define TEMP_SENSOR_ADDRESS 0x48
#define TEMPERATURE_STATUS_POSITION 4
#define TEMPERATURE_DATA_ARRAY_SIZE 1

class Temperature : public ISensor {
public:
    void initSensor();
    void readData();
    uint8_t dataLength();
    float *getData();
    SensorStatus getStatus();

private:
    TMP102 *temp_sensor;
    float temp;
    float data[TEMPERATURE_DATA_ARRAY_SIZE];
};

#endif