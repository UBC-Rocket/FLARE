#pragma once

/**
 * Barometer Sensor Class
 */

/*Includes------------------------------------------------------------*/
#include "../env_config.h"
#include "../sensor_data.hpp"
#include "sensors_interface.h"
#include <cstdint>

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
    const uint8_t BAROMETER_SENSOR_ID = 0x03;
    DataSpoof<2> dat{BAROMETER_SENSOR_ID, data_};
};
