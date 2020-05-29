#ifndef ACCELEROMETER_H_6BED1C38732148D78896802C9099D584
#define ACCELEROMETER_H_6BED1C38732148D78896802C9099D584

/**
 * Accelerometer Sensor Class
 */

/*Includes------------------------------------------------------------*/
#include "sensors-interface.h"

class Accelerometer : public SensorBase<3> {
  public:
    void readData(){};
    SensorStatus getStatus() { return SensorStatus::NOMINAL; }

  private:
    int16_t x, y, z;
};

#endif
