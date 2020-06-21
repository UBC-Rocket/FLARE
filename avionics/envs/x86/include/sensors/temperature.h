#pragma once

/**
 * Temperature Sensor Class
 */

/*Includes------------------------------------------------------------*/
#include "sensors_interface.h"

/*Constants------------------------------------------------------------*/

class Temperature : public SensorBase<1> {
    //   private:
    //     static constexpr int TEMPERATURE_DATA_ARRAY_SIZE = 1;

  public:
    Temperature(float *const data) : SensorBase(data) { data_[0] = 20; }
    void initSensor() {}
    void readData() {}
    // uint8_t dataLength() { return TEMPERATURE_DATA_ARRAY_SIZE; }
    // float *getData() { return data; }
    SensorStatus getStatus() { return SensorStatus::NOMINAL; }

    //   private:
    // float data[TEMPERATURE_DATA_ARRAY_SIZE] = {20};
};
