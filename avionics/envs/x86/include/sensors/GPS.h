#ifndef SENSORS__GPS_H_C87D138269654043A56EFCE31F6A40C5
#define SENSORS__GPS_H_C87D138269654043A56EFCE31F6A40C5

#include "HAL/port_impl.h"
#include "sensors-interface.h"

/*Variables------------------------------------------------------------*/

class GPS : public SensorBase<3> {
  public:
    GPS(Hal::Serial &seri, float *buf) : SensorBase(buf) {
        buf[0] = 49;
        buf[1] = -123;
        buf[2] = 0;
    }
    void initSensor() {}
    void readData() {}
    SensorStatus getStatus() { return SensorStatus::NOMINAL; }
};

#endif
