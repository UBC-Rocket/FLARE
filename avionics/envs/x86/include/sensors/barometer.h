#ifndef SENSORS__BAROMETER_H_8294D8F9282A4D1DB60BC9F067B8FE5A
#define SENSORS__BAROMETER_H_8294D8F9282A4D1DB60BC9F067B8FE5A

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
    Barometer(float *buf) : SensorBase(buf) {}
    void initSensor() {}
    void readData() {
        dat.getData(); // Forces DataSpoof to refresh.
    }
    SensorStatus getStatus() { return SensorStatus::NOMINAL; }

  private:
    DataSpoof<2> dat{BAROMETER_DATA, data_};
};

#endif
