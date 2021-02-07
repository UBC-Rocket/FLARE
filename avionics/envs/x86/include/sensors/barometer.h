#pragma once

/**
 * Barometer Sensor Class
 */

/*Includes------------------------------------------------------------*/
#include "../sensor_data.hpp"
#include "sensors_interface.h"

/*Constants------------------------------------------------------------*/

class Barometer : public SensorBase<BAROMETER_DATA_LENGTH> {
  public:
    Barometer(float *const buf) : SensorBase(buf) {}
    void initSensor() {}
    void readData() {
        dat.getData(); // Forces DataSpoof to refresh.
    }
    SensorStatus getStatus() { return SensorStatus::NOMINAL; }

  private:
    DataSpoof<BAROMETER_DATA_LENGTH> dat{SensorType::BAROMETER, data_};
};
