#pragma once

#include "../sensor_data.hpp"
#include "sensors_interface.h"

/*Variables------------------------------------------------------------*/

class GPS : public SensorBase<GPS_DATA_LENGTH> {
  public:
    GPS(Hal::Serial &, float *const buf) : SensorBase(buf) {
    }
    void initSensor() {}
    void readData() {
        dat.getData();
    }
    SensorStatus getStatus() { return SensorStatus::NOMINAL; }

  private:
    DataSpoof<GPS_DATA_LENGTH> dat{SensorType::GPS, data_};
};
