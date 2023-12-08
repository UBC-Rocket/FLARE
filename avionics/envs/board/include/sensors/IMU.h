#pragma once
/**
 * IMU Sensor Class
 */

/*Includes------------------------------------------------------------*/
#include "BMI088.h"
#include "options.h"
#include "sensors_interface.h"

/*Constants------------------------------------------------------------*/
// #define IMU_STATUS_POSITION 5

class IMU : public SensorBase<IMU_DATA_LENGTH> {
  public:
    IMU(float *const data);
    void readData();

  private:
    BMI088 bmi088; 
    float ax;
    float ay;
    float az;
    float gx;
    float gy;
    float gz;
    int16_t imuTemp;
};
