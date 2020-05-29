#ifndef SENSORS__IMU_H_DC4773AC6EA440C5967FDBBE32B8B196
#define SENSORS__IMU_H_DC4773AC6EA440C5967FDBBE32B8B196

/**
 * IMU Sensor Class
 */

/*Includes------------------------------------------------------------*/
#include "sensors-interface.h"

class IMU : public SensorBase<4> {
    //   private:
    //     static constexpr int IMU_DATA_ARRAY_SIZE = 4;

  public:
    IMU(float *buf) : SensorBase(buf) {
        buf[0] = 1;
        buf[1] = 0;
        buf[2] = 0;
        buf[3] = 0;
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

#endif
