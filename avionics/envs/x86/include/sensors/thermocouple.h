#ifndef SENSORS__THERMOCOUPLE_H_0DAE15EFDA86479BB30DDA9EF7AC1016
#define SENSORS__THERMOCOUPLE_H_0DAE15EFDA86479BB30DDA9EF7AC1016

/**
  * Thermocouple Sensor Class
  */

/*Includes------------------------------------------------------------*/
#include "sensors-interface.h"

/*Constants------------------------------------------------------------*/

class Thermocouple : public ISensor {
private:
    static constexpr int THERMOCOUPLE_DATA_ARRAY_SIZE = 1;
public:
    void initSensor(){}
    void readData(){}
    uint8_t dataLength(){ return THERMOCOUPLE_DATA_ARRAY_SIZE; }
    float *getData(){ return data; }
    SensorStatus getStatus(){ return SensorStatus::NOMINAL; }

private:
    float data[THERMOCOUPLE_DATA_ARRAY_SIZE] = {200}; //thermocouples are in hot places
};

#endif
