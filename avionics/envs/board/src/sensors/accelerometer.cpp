/*Includes------------------------------------------------------------*/
#include "options.h"
#include "sensors/accelerometer.h"

Accelerometer::Accelerometer(float *const buf) : SensorBase(buf) {
/*init accerlerometer*/
#ifdef TESTING
    SerialUSB.println("Initializing accelerometer");
#endif
    // replaced with below for tantalus lite version
    // accelerometer.setI2CAddr(ACCELEROMETER_ADDRESS);
    // accelerometer.begin(LIS331::USE_I2C);
    // accelerometer.setFullScale(LIS331::HIGH_RANGE);
    
    Wire.begin();                   // Initiate the Wire library
    Wire.beginTransmission(0x53);   // Start communicating with the device (ADXL345 I2C address)
    Wire.write(0x2D);               // Access/ talk to POWER_CTL Register - 0x2D
    Wire.write(8);                  // (8dec -> 0000 1000 binary) Bit D3 High for measuring enable 
    Wire.endTransmission();
    delay(10);

    status = SensorStatus::NOMINAL;
}

void Accelerometer::getRawAccel(int16_t* ax, int16_t* ay, int16_t* az) {
  Wire.beginTransmission(0x53); // ADXL345 I2C address
  Wire.write(0x32); // Start with register 0x32 (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(0x53, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  *ax = ( Wire.read()| Wire.read() << 8); // X-axis value
  *ay = ( Wire.read()| Wire.read() << 8); // Y-axis value
  *az = ( Wire.read()| Wire.read() << 8); // Z-axis value
}

int Accelerometer::twosComplementToInt(int16_t input) { // takes a 16 bit 2s complement integer and 
  return (input >= 32768) ? (int)(-(~input + 1)) : (int)input;
}

void Accelerometer::readData(void) {
#ifdef TESTING
    SerialUSB.println("Polling accelerometer");
#endif
    // modified for tantalus lite version
    // accelerometer.readAxes(x, y, z);
    // data_[0] = accelerometer.convertToG(ACCELEROMETER_SCALE, x);
    // data_[1] = accelerometer.convertToG(ACCELEROMETER_SCALE, y);
    // data_[2] = accelerometer.convertToG(ACCELEROMETER_SCALE, z);
    getRawAccel(&x, &y, &z);
    data_[0] = twosComplementToInt(x) / 256;
    data_[1] = twosComplementToInt(y) / 256;
    data_[2] = twosComplementToInt(z) / 256;
}
