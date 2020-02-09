#ifndef ACCELEROMETER_H_6BED1C38732148D78896802C9099D584
#define ACCELEROMETER_H_6BED1C38732148D78896802C9099D584

/**
  * Accelerometer Sensor Class
  */

/*Includes------------------------------------------------------------*/
#include "sensors-interface.h"

class Accelerometer : public ISensor {
private:
    static constexpr int ACCELEROMETER_DATA_ARRAY_SIZE = 3;

public:
    void initSensor(){}
    void readData(){};
    uint8_t dataLength(){return ACCELEROMETER_DATA_ARRAY_SIZE;}
    float *getData() {return data;}
    SensorStatus getStatus() {return SensorStatus::NOMINAL;}

private:
    int16_t x, y, z;
    float data[ACCELEROMETER_DATA_ARRAY_SIZE] = {0, 0, 9.8};
};

#endif
