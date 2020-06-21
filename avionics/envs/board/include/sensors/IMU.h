#pragma once
/**
 * IMU Sensor Class
 */

/*Includes------------------------------------------------------------*/
#include "Adafruit_BNO055.h"
#include "options.h"
#include "sensors_interface.h"

/*Constants------------------------------------------------------------*/
// #define IMU_STATUS_POSITION 5

class IMU : public SensorBase<4> {
  public:
    IMU(float *const data);
    void readData();

  private:
    constexpr static uint8_t IMU_ADDRESS = 0x28;
    Adafruit_BNO055 bno055; //{-1, IMU_ADDRESS};
};
