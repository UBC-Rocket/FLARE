#ifndef SENSORS__BAROMETER_H_8294D8F9282A4D1DB60BC9F067B8FE5A
#define SENSORS__BAROMETER_H_8294D8F9282A4D1DB60BC9F067B8FE5A

/**
  * Barometer Sensor Class
  */

/*Includes------------------------------------------------------------*/
#include "sensors-interface.h"
#include "../sensor_data.hpp"
#include "../env_config.h"

/*Constants------------------------------------------------------------*/


class Barometer : public ISensor {
private:
    static constexpr int BAROMETER_DATA_ARRAY_SIZE = 3;

public:
    void initSensor(){}
    void readData(){};
    uint8_t dataLength(){return BAROMETER_DATA_ARRAY_SIZE;}
    float *getData() {return data;}
    SensorStatus getStatus() {return SensorStatus::NOMINAL;}

private:
    DataSpoof<1> dat{BAROMETER_DATA};
    float data[BAROMETER_DATA_ARRAY_SIZE] = {1000, 20};
};

#endif
