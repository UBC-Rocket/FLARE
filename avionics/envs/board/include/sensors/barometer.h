#ifndef BAROMETER_HPP_E5BACDD72523463B88A01C9A5C9F31FE
#define BAROMETER_HPP_E5BACDD72523463B88A01C9A5C9F31FE

/**
 * Barometer Sensor Class
 */

/*Includes------------------------------------------------------------*/
#include "MS5xxx.h" // Barometer
#include "sensors_interface.h"

/*Constants------------------------------------------------------------*/
// #define BAROMETER_STATUS_POSITION 3
// #define BAROMETER_DATA_ARRAY_SIZE 2

class Barometer : public SensorBase<BAROMETER_DATA_LENGTH> {
  public:
    Barometer(float *const buf);
    void readData();

  private:
    MS5xxx barometer;
    int fakePressure;
    int negative;
};

#endif
