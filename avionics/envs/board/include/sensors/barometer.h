#ifndef BAROMETER_HPP_E5BACDD72523463B88A01C9A5C9F31FE
#define BAROMETER_HPP_E5BACDD72523463B88A01C9A5C9F31FE

/**
 * Barometer Sensor Class
 */

/*Includes------------------------------------------------------------*/
#include "options.h"
#include "MS5803_01.h" // Barometer
#include "sensors_interface.h"
#include <Adafruit_BMP085.h> // tantalus lite version barometer
#include "../MS5611.h"

/*Constants------------------------------------------------------------*/
// #define BAROMETER_STATUS_POSITION 3
// #define BAROMETER_DATA_ARRAY_SIZE 2

class Barometer : public SensorBase<BAROMETER_DATA_LENGTH> {
  public:
    Barometer(float *const buf);
    void readData();

  private:
    // replaced for tantalus lite version
    // MS_5803 barometer;
#ifdef STAGE2
    MS5611 bmp;
#else
    Adafruit_BMP085 bmp;
#endif
};

#endif
