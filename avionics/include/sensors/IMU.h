#ifndef IMU_H
#define IMU_H

/**
 * IMU Sensor Class
 */

/*Includes------------------------------------------------------------*/
#include "sensors-interface.h"
#include "Adafruit_BNO055.h"

/*Constants------------------------------------------------------------*/
#define IMU_STATUS_POSITION 5
#define IMU_ADDRESS 0x28
#if defined NOSECONE
    #define IMU_DATA_ARRAY_SIZE 3
#elif defined BODY
    #define IMU_DATA_ARRAY_SIZE 9
#endif

class IMU : public ISensor {
    public:
    SensorStatus initSensor();
    SensorStatus readData(float* data);
};

#endif