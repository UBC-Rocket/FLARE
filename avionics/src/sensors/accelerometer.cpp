/*Includes------------------------------------------------------------*/
#include "sensors/accelerometer.h"

SensorStatus Accelerometer::initSensor() {
    /*init accerlerometer*/
    #ifdef TESTING
        SerialUSB.println("Initializing accelerometer");
    #endif
    accelerometer.setI2CAddr(ACCELEROMETER_ADDRESS);
    accelerometer.begin(LIS331::USE_I2C);
    accelerometer.setFullScale(LIS331::HIGH_RANGE);

    return NOMINAL;
}

SensorStatus Accelerometer::readData(float* data) {
    #ifdef TESTING
        SerialUSB.println("Polling accelerometer");
    #endif
    accelerometer.readAxes(x, y, z);
    data[0] = accelerometer.convertToG(ACCELEROMETER_SCALE, x);
    data[1] = accelerometer.convertToG(ACCELEROMETER_SCALE, y);
    data[2] = accelerometer.convertToG(ACCELEROMETER_SCALE, z);

    return NOMINAL;
}