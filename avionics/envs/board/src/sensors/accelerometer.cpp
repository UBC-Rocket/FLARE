/*Includes------------------------------------------------------------*/
#include "sensors/accelerometer.h"

Accelerometer::Accelerometer(float *const buf) : SensorBase(buf) {
/*init accerlerometer*/
#ifdef TESTING
    SerialUSB.println("Initializing accelerometer");
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
    SerialUSB.println("Polling accelerometer");
    SerialUSB.print("x:  ");
    SerialUSB.println(data_[0]);
    SerialUSB.print("y: ");
    SerialUSB.println(data_[1]);
    SerialUSB.print("z: ");
    SerialUSB.println(data_[2]);
#endif
}
