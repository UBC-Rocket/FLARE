#pragma once
/**
 * Accelerometer Sensor Class
 */

/*Includes------------------------------------------------------------*/
#include "sensors_interface.h"

class Accelerometer : public SensorBase<3> {
  public:
    Accelerometer(float *const buf) : SensorBase(buf) {}
    void readData(){};
    SensorStatus getStatus() { return SensorStatus::NOMINAL; }

  private:
    int16_t x, y, z;
};
