#pragma once

#include "HAL/port_impl.h"
#include "../sensor_data.hpp"
#include "sensors_interface.h"

/*Variables------------------------------------------------------------*/

class GPS : public SensorBase<3> {
  public:
    GPS(Hal::Serial &, float *const buf) : SensorBase(buf) {
    }
    void initSensor() {}
    void readData() {
        dat.getData();
    }
    SensorStatus getStatus() { return SensorStatus::NOMINAL; }

  private:
    const uint8_t GPS_SENSOR_ID = 0x00;
    DataSpoof<3> dat{GPS_SENSOR_ID, data_};
};
