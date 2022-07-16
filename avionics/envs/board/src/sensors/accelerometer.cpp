/*Includes------------------------------------------------------------*/
#include "options.h"
#include "sensors/accelerometer.h"
#include "Wire.h"

Accelerometer::Accelerometer(float *const buf) : SensorBase(buf) {
/*init accerlerometer*/
#ifdef TESTING
    SerialUSB.println("Initializing accelerometer");
#endif
    // replaced with below for tantalus lite version
    // accelerometer.setI2CAddr(ACCELEROMETER_ADDRESS);
    // accelerometer.begin(LIS331::USE_I2C);
    // accelerometer.setFullScale(LIS331::HIGH_RANGE);
    
    WireKinetis.begin();                   // Initiate the Wire library
    WireKinetis.beginTransmission(0x53);   // Start communicating with the device (ADXL345 I2C address)
    WireKinetis.write(0x2D);               // Access/ talk to POWER_CTL Register - 0x2D
    WireKinetis.write(8);                  // (8dec -> 0000 1000 binary) Bit D3 High for measuring enable 
    WireKinetis.endTransmission();
    delay(10);

    byte _s = B00000011; // stole more code from SEEED, this sets the range to +/-16g
    byte _b;

    readFromReg(0x53, 1, &_b);
    _s |= (_b & B11101100);
    writeToReg(0x53, _s);

    status = SensorStatus::NOMINAL;
}

void Accelerometer::readFromReg(byte address, int num, byte _buff[]) { // stolen from the SEEED ADXL345 library
  WireKinetis.beginTransmission(0x53); // start transmission to device
  WireKinetis.write(address);             // sends address to read from
  WireKinetis.endTransmission();         // end transmission

  WireKinetis.beginTransmission(0x53); // start transmission to device
  WireKinetis.requestFrom(0x53, num);    // request 6 bytes from device

  int i = 0;
  while (WireKinetis.available()) {      // device may send less than requested (abnormal)
    _buff[i] = WireKinetis.read();    // receive a byte
    i++;
  }
  WireKinetis.endTransmission();         // end transmission
}
void Accelerometer::writeToReg(byte address, byte val) {
  WireKinetis.beginTransmission(0x53); // start transmission to device
  WireKinetis.write(address);             // send register address
  WireKinetis.write(val);                 // send value to write
  WireKinetis.endTransmission();         // end transmission
}

void Accelerometer::getRawAccel(int16_t* ax, int16_t* ay, int16_t* az) {
  WireKinetis.beginTransmission(0x53); // ADXL345 I2C address
  WireKinetis.write(0x32); // Start with register 0x32 (ACCEL_XOUT_H)
  WireKinetis.endTransmission(false);
  WireKinetis.requestFrom(0x53, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  *ax = ( WireKinetis.read()| WireKinetis.read() << 8); // X-axis value
  *ay = ( WireKinetis.read()| WireKinetis.read() << 8); // Y-axis value
  *az = ( WireKinetis.read()| WireKinetis.read() << 8); // Z-axis value
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
