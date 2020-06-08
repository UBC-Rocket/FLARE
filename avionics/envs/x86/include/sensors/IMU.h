#pragma once

/**
 * IMU Sensor Class
 */

/*Includes------------------------------------------------------------*/
#include "sensors-interface.h"

class IMU : public SensorBase<4> {
    //   private:
    //     static constexpr int IMU_DATA_ARRAY_SIZE = 4;

  public:
    IMU(float *const data) : SensorBase(data) {
        data_[0] = 1;
        data_[1] = 0;
        data_[2] = 0;
        data_[3] = 0;
    }
    void initSensor() {}
    void readData() {}
    // uint8_t dataLength() { return IMU_DATA_ARRAY_SIZE; }
    // float *getData() { return data; }
    SensorStatus getStatus() { return SensorStatus::NOMINAL; }

    //   private:
    //     float data[IMU_DATA_ARRAY_SIZE] = {1, 0, 0, 0}; // Unit quaternion,
    //     for now
};
