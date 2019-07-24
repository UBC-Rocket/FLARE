/*************************** MAX31855k.cpp *****************************
 * Copyright (c) 2015 SparkFun Electronics                                     *
 *                                                                             *
 * Permission is hereby granted, free of charge, to any person obtaining a     *
 * copy of this software and associated documentation files (the "Software"),  *
 * to deal in the Software without restriction, including without limitation   *
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,    *
 * and/or sell copies of the Software, and to permit persons to whom the       *
 * Software is furnished to do so, subject to the following conditions:        *
 *                                                                             *
 * The above copyright notice and this permission notice shall be included in  *
 * all copies or substantial portions of the Software.                         *
 *                                                                             *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,    *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER      *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING     *
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER         *
 * DEALINGS IN THE SOFTWARE.                                                   *
 *                                                                             *
 *                                                                             *
 * Library to read temperature from a MAX31855K type K thermocouple digitizer  *
 * Implementation of functions defined in MAX31855k.h                          *
 ******************************************************************************/

#include "MAX31855k.h"
#include "sensors.h"

////////////////////////////////////////////////////////////////////////////////
// Description  : This constructor does the required setup
// Input        : uint8_t _cs: The Arduino pin number of the chip select line
//                for this instance
//                uint8_t _sck: The Arduino pin number of the SPI clock
//                uint8_t _so:  The Arduino pin number of the SPI MISO
//                uint8_t _vcc: The Arduino pin number to source the power
//                uint8_t _gnd: The Arduino pin number to sink the power
// Output       : Instance of this class with pins configured
// Return       : None
// Usage        : MAX31855k <name>(<pinNumber>);
////////////////////////////////////////////////////////////////////////////////
MAX31855k::MAX31855k(const uint8_t _cs, const uint8_t _vcc,
    const uint8_t _gnd, const bool _debug) : cs(_cs), debug(_debug)
{
  // Redundant with SPI library if using default SS
  pinMode(cs, OUTPUT);
  // SCK & MOSI set in SPI library, MISO autoconfigures

  SPIThermo.begin();
  digitalWrite(cs, HIGH);

  // Setup two GPIOs as power and ground
  if (_gnd != NONE)
  {
    pinMode(_gnd, OUTPUT);
    digitalWrite(_gnd, LOW);
  }
  if (_vcc != NONE)
  {
    pinMode(_vcc, OUTPUT);
    digitalWrite(_vcc, HIGH);
  }
}

////////////////////////////////////////////////////////////////////////////////
// Change the cs pin
////////////////////////////////////////////////////////////////////////////////
void MAX31855k::setCS(int pin)
{
  digitalWrite(cs, HIGH); // Make sure to set old cs high to disable the chip
  cs = pin;               //change to new cs pin
  pinMode(cs, OUTPUT);
  digitalWrite(cs, HIGH);
}

////////////////////////////////////////////////////////////////////////////////
// Deconstructor does nothing.  It's up to the user to re-assign
// chip select pin if they want to use it for something else.  We don't call
// SPI.end() in case there is another SPI device we don't want to kill.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Description  : This function reads the cold junction temperature
// Input        : None
// Output       : Loads member variables with data from the IC
// Return:      : None
// Usage        : <objectName>.readBytes();
// MAX31855K Memory Map:
//  D[31:18] Signed 14-bit thermocouple temperature data
//  D17      Reserved: Always reads 0
//  D16      Fault: 1 when any of the SCV, SCG, or OC faults are active, else 0
//  D[15:4]  Signed 12-bit internal temperature
//  D3       Reserved: Always reads 0
//  D2       SCV fault: Reads 1 when thermocouple is shorted to V_CC, else 0
//  D1       SCG fault: Reads 1 when thermocouple is shorted to gnd, else 0
//  D0       OC  fault: Reads 1 when thermocouple is open-circuit, else 0
////////////////////////////////////////////////////////////////////////////////
void MAX31855k::readBytes(void)
{
  digitalWrite(cs, LOW);

  SPIThermo.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));  // Defaults
  data.bytes[3] = SPIThermo.transfer(0x00);
  data.bytes[2] = SPIThermo.transfer(0x00);
  data.bytes[1] = SPIThermo.transfer(0x00);
  data.bytes[0] = SPIThermo.transfer(0x00);
  SPIThermo.endTransaction();

  digitalWrite(cs, HIGH);

  return;
}

////////////////////////////////////////////////////////////////////////////////
// Description  : This function reads the current temperature
// Input        : MAX31855k::units _u: The units of temperature to
//                  return. C (default), K, R, or F
// Output       : Error messages before freezing
// Return:      : float: The temperature in requested units.
// Usage        : float tempC = <objectName>.read_temp();
//                float tempC = <objectName>.read_temp(MAX31855k::C);
//                float tempF = <objectName>.read_temp(MAX31855k::F);
//                float tempK = <objectName>.read_temp(MAX31855k::K);
//                float tempR = <objectName>.read_temp(MAX31855k::R);
////////////////////////////////////////////////////////////////////////////////
float MAX31855k::readTemp(MAX31855k::units _u)
{
  int16_t value;
  float temp;

  readBytes();

  if (checkHasFault()) {
    return NAN;
  } else {
    // Bits D[31:18] are the signed 14-bit thermocouple temperature value
    if (data.uint32 & ((uint32_t)1 << 31)) { // Sign extend negative numbers
      value = 0xC000 | ((data.uint32 >> 18) & 0x3FFF);
    } else {
      value = data.uint32 >> 18; // Shift off all but the temperature data
    }
  }

  temp = value/4.0;
  switch (_u) {
  case F:
    temp = (temp * 9.0 / 5.0) + 32.0;
    break;
  case K:
    temp += 273.15;
    break;
  case R:
    temp = (temp + 273.15) * 9.0 / 5.0;
  case C:
  default:
    break;
  }

  return temp;
}

////////////////////////////////////////////////////////////////////////////////
// Description  : This function reads the cold junction temperature
// Input        : None
// Output       : None
// Return:      : float: The temperature in ˚C
// Usage        : float tempC = <objectName>.readCJT();
////////////////////////////////////////////////////////////////////////////////
float MAX31855k::readCJT(void)
{
  float ret;

  readBytes();

  if (checkHasFault())
  {
    return NAN;
  }

  if (data.uint32 & ((int32_t)1 << 15))
  { // Sign extend negative numbers
    ret = 0xF000 | ((data.uint32 >> 4) & 0xFFF);
  }
  else
  {
    ret = (data.uint32 >> 4) & 0xFFF;
  }

  return ret/16;
}

////////////////////////////////////////////////////////////////////////////////
// Description  : This function checks the fault bits from the MAX31855K IC
// Input        : None
// Output       : Serial prints debug mesages if debug == true
// Return:      : Fault bits that were high, or 8 for unknow & 0 for no faults
// Usage        : checkHasFault();
////////////////////////////////////////////////////////////////////////////////
uint8_t MAX31855k::checkHasFault(void)
{
  if (!data.uint32) {
    // If all bits are low, either it's not wired right, or we actually measured
    // 0˚.  There is no way to tell. With debug turned on this will warn.
    if (debug)
      SerialUSB.println(F("\nMAX31855K::All bits were zero.  Fishy..."));
  }

  if (data.uint32 & ((uint32_t)1<<16)) { // Bit D16 is high => fault
    if (data.uint32 & 1) {
      if (debug){
        SerialUSB.println(data.uint32);
        SerialUSB.println(F("\nMAX31855K::OC Fault: No Probe"));
      }
      return 0b1;
    } else if (data.uint32 & (1<<1)) {
      if (debug)
        SerialUSB.println(F("\nMAX31855K::SCG Fault: Thermocouple is shorted to GND"));
      return 0b10;
    } else if (data.uint32 & (1<<2)) {
      if (debug)
        SerialUSB.println(F("\nMAX31855K::SCV Fault: Thermocouple is shorted to VCC"));
      return 0b100;
    } else {
      if (debug)
        SerialUSB.println(F("\nMAX31855K::Unknown Fault"));
      return 0b1000;
    }
  } else {
    return 0; // No fault
  }
}
