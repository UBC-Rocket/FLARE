/*Includes------------------------------------------------------------*/
#include "sensors/accelerometer.h"
#include "options.h"
#include "Arduino.h"

Accelerometer::Accelerometer(float *const buf) : SensorBase(buf) {
/*init accerlerometer*/
#ifdef TESTING
    Serial.println("Initializing accelerometer");
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
    Serial.println("Polling accelerometer");
    Serial.print("x:  ");
    Serial.println(data_[0]);
    Serial.print("y: ");
    Serial.println(data_[1]);
    Serial.print("z: ");
    Serial.println(data_[2]);
#endif
}
