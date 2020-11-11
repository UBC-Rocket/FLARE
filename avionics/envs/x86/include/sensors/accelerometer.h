#pragma once
/**
 * Accelerometer Sensor Class
 */

/*Includes------------------------------------------------------------*/
#include "../sensor_data.hpp"
#include "sensors_interface.h"

class Accelerometer : public SensorBase<ACCELEROMETER_DATA_LENGTH> {
  public:
    Accelerometer(float *const buf) : SensorBase(buf) {}
    void readData(){
      dat.getData(); // Forces DataSpoof to refresh.
    }
    SensorStatus getStatus() { return SensorStatus::NOMINAL; }

  private:
    DataSpoof<ACCELEROMETER_DATA_LENGTH> dat{SensorType::ACCELEROMETER, data_};
};
