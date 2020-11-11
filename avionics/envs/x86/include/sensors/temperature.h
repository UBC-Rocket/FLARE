#pragma once

/**
 * Temperature Sensor Class
 */

/*Includes------------------------------------------------------------*/
#include "../sensor_data.hpp"
#include "sensors_interface.h"

/*Constants------------------------------------------------------------*/

class Temperature : public SensorBase<TEMPERATURE_DATA_LENGTH> {

  public:
    Temperature(float *const data) : SensorBase(data) { data_[0] = 20; }
    void initSensor() {}
    void readData() {
        dat.getData(); // Forces DataSpoof to refresh.
    }

    SensorStatus getStatus() { return SensorStatus::NOMINAL; }
  
  private:
    DataSpoof<TEMPERATURE_DATA_LENGTH> dat{SensorType::TEMPERATURE, data_};
};
