#pragma once

#include "HAL/port_impl.h"
#include "sensors-interface.h"

/*Variables------------------------------------------------------------*/

class GPS : public SensorBase<3> {
  public:
    GPS(Hal::Serial &seri, float *const buf) : SensorBase(buf) {
        data_[0] = 49;
        data_[1] = -123;
        data_[2] = 0;
    }
    void initSensor() {}
    void readData() {}
    SensorStatus getStatus() { return SensorStatus::NOMINAL; }
};
