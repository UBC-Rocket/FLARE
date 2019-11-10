#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

/**
  * Accelerometer Sensor Class
  */

/*Includes------------------------------------------------------------*/
#include "sensors-interface.h"
#include "SparkFun_LIS331.h" // Accelerometer

/*Constants------------------------------------------------------------*/
#define ACCELEROMETER_ADDRESS 0x18
#define ACCELEROMETER_SCALE 24
#define ACCELEROMETER_STATUS_POSITION 2
#define ACCELEROMETER_DATA_ARRAY_SIZE 3

class Accelerometer : public ISensor {
public:
    void initSensor();
    void readData(void);
    uint8_t dataLength();
    float *getData();
    SensorStatus getStatus();

private:
    LIS331 accelerometer;
    int16_t x, y, z;
    float data[ACCELEROMETER_DATA_ARRAY_SIZE];
};
#endif