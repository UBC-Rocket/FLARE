#pragma once

/**
 * Barometer Sensor Class
 */

/*Includes------------------------------------------------------------*/
#include "../env_config.h"
#include "../sensor_data.hpp"
#include "sensors-interface.h"

/*Constants------------------------------------------------------------*/

class Barometer : public SensorBase<2> {
  public:
    Barometer(float *const buf) : SensorBase(buf) {}
    void initSensor() {}
    void readData() {
        dat.getData(); // Forces DataSpoof to refresh.
    }
    SensorStatus getStatus() { return SensorStatus::NOMINAL; }

  private:
    DataSpoof<2> dat{BAROMETER_DATA, data_};
};
