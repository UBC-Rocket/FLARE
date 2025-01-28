/*Includes------------------------------------------------------------*/
#include "sensors/accelerometer.h"
#include "options.h"
#include "Arduino.h"
#include "log.hpp"

Accelerometer::Accelerometer(float *const buf) : SensorBase(buf) {
/*init accerlerometer*/
#ifdef TESTING
    LOG_DEBUG("Initializing accelerometer");
#endif
    accelerometer.setI2CAddr(ACCELEROMETER_ADDRESS);
    accelerometer.begin(LIS331::USE_I2C);
    //accelerometer.setFullScale(LIS331::HIGH_RANGE);

    status = SensorStatus::NOMINAL;
}

void Accelerometer::readData(void) {
    accelerometer.readAxes(x, y, z);
    data_[0] = accelerometer.convertToG(ACCELEROMETER_SCALE, x);
    data_[1] = accelerometer.convertToG(ACCELEROMETER_SCALE, y);
    data_[2] = accelerometer.convertToG(ACCELEROMETER_SCALE, z);

#ifdef TESTING
    LOG_DEBUG("Polling accelerometer");
    LOG_DEBUG(("x: " + std::to_string(data_[0])).c_str());
    LOG_DEBUG(("y: " + std::to_string(data_[1])).c_str());
    LOG_DEBUG(("z: " + std::to_string(data_[2])).c_str());
#endif
}
