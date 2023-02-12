/*Includes------------------------------------------------------------*/
#include "sensors/temperature.h"

Temperature::Temperature(float *const data)
    : SensorBase(data) {

/*init temp sensor*/
#ifdef TESTING
    SerialUSB.println("Initializing temperature sensor");
#endif
    // Start I2C Transmission
    Wire.beginTransmission(0x48);
    // Select configuration register
    Wire.write(0x01);
    // Continuous conversion mode, Power-up default
    Wire.write(0x60);
    // Stop I2C Transmission
    Wire.endTransmission();

    status = SensorStatus::NOMINAL;
}

void Temperature::readData() {
    unsigned int tempData[2];

    // Starts I2C communication
    Wire.beginTransmission(0x48);
    // Select data register
    Wire.write(0x00);
    // Stop I2C transmission
    Wire.endTransmission();

    // Request 2 bytes of data
    Wire.requestFrom(0x48, 2);

    // Read 2 bytes of data
    // temp msb, temp lsb
    if (Wire.available() == 2) {
        tempData[0] = Wire.read();
        tempData[1] = Wire.read();
    }

    // Convert the data to 12-bits
    int temp = ((tempData[0] * 256) + tempData[1]) / 16.0;
    if (temp > 2047) {
        temp -= 4096;
    }
    float cTemp = temp * 0.0625;
    data_[0] = cTemp;

#ifdef TESTING
    SerialUSB.println("Polling temperature sensor");
    SerialUSB.print("cTemp:  ");
    SerialUSB.println(data_[0]);
#endif
}
