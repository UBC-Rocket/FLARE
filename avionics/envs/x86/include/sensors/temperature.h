#ifndef SENSORS__TEMPERATURE_H_C47C3CE2B03B4C5BB928707B66E7052D
#define SENSORS__TEMPERATURE_H_C47C3CE2B03B4C5BB928707B66E7052D

/**
 * Temperature Sensor Class
 */

/*Includes------------------------------------------------------------*/
#include "sensors-interface.h"

/*Constants------------------------------------------------------------*/

class Temperature : public SensorBase<1> {
    //   private:
    //     static constexpr int TEMPERATURE_DATA_ARRAY_SIZE = 1;

  public:
    Temperature(float *buf) : SensorBase(buf) { buf[0] = 20; }
    void initSensor() {}
    void readData() {}
    // uint8_t dataLength() { return TEMPERATURE_DATA_ARRAY_SIZE; }
    // float *getData() { return data; }
    SensorStatus getStatus() { return SensorStatus::NOMINAL; }

    //   private:
    // float data[TEMPERATURE_DATA_ARRAY_SIZE] = {20};
};
#endif
