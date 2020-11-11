#pragma once

/**
 * IMU Sensor Class
 */

/*Includes------------------------------------------------------------*/
#include "../sensor_data.hpp"
#include "sensors_interface.h"

class IMU : public SensorBase<IMU_DATA_LENGTH> {

  public:
    IMU(float *const data) : SensorBase(data) {}
    void initSensor() {}
    void readData() {
        dat.getData(); // Forces DataSpoof to refresh.
    }
    SensorStatus getStatus() { return SensorStatus::NOMINAL; }

  private:
    DataSpoof<IMU_DATA_LENGTH> dat{SensorType::IMU, data_};
};
