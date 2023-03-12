// /********************************** MAX31855k.h ********************************
//  * Copyright (c) 2015 SparkFun Electronics                                     *
//  *                                                                             *
//  * Permission is hereby granted, free of charge, to any person obtaining a     *
//  * copy of this software and associated documentation files (the "Software"),  *
//  * to deal in the Software without restriction, including without limitation   *
//  * the rights to use, copy, modify, merge, publish, distribute, sublicense,    *
//  * and/or sell copies of the Software, and to permit persons to whom the       *
//  * Software is furnished to do so, subject to the following conditions:        *
//  *                                                                             *
//  * The above copyright notice and this permission notice shall be included in  *
//  * all copies or substantial portions of the Software.                         *
//  *                                                                             *
//  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  *
//  * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,    *
//  * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE *
//  * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER      *
//  * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING     *
//  * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER         *
//  * DEALINGS IN THE SOFTWARE.                                                   *
//  *                                                                             *
//  *                                                                             *
//  * Library to read temperature from a MAX6675 type K thermocouple digitizer    *
//  *                                                                             *
//  * SO output format:                                                           *
//  * 16 bits                                                                     *
//  * The first bit, D15, is a dummy sign bit and is always zero.                 *
//  * Bits D14:D3 contain the converted temperature in the order of MSB to LSB.   *
//  * Bit D2 is normally low and goes high when the thermocouple input is open.   *
//  * D1 is low to provide a device ID for the MAX6675 and bit D0 is tri-state.   *
//  *                                                                             *
//  * The method read_temp(max31855k::unit) takes a parameter which determines    *
//  * the unit of temperature returned.                                           *
//  * max31855k::C will return degrees Celsius                                    *
//  * max31855k::K will return degrees Kelvin                                     *
//  * max31855k::F will return degrees Fahrenheit                                 *
//  * max31855k::R will return degrees Rankine                                    *
//  *******************************************************************************/
// #pragma once

// #include <Arduino.h>
// // #include "sensors.h"
// #include <SPI.h> // Have to include this in the main sketch too... (Using SPI)

// /*Constants------------------------------------------------------------*/
// #define THERMO_SELECT_PIN 6

// const uint8_t NONE = 0; // This is used to indicate VCC or GND pin isn't used

// class MAX31855k {
//   protected:
//     union { // Union makes conversion from 4 bytes to an unsigned 32-bit int
//             // easy
//         uint8_t bytes[4];
//         uint32_t uint32;
//     } data;
//     uint8_t cs;

//     void readBytes(void);
//     uint8_t checkHasFault(void);

//   public:
//     // Simple Arduino API style guide functions
//     inline float readTempC() { return readTemp(MAX31855k::C); }
//     inline float readTempF() { return readTemp(MAX31855k::F); }
//     inline float readTempR() { return readTemp(MAX31855k::R); }
//     inline float readTempK() { return readTemp(MAX31855k::K); }

//     // More advanced code concepts used below
//     enum units { F, C, K, R };
//     // If non-zero will turn on serial debugging messages
//     uint8_t debug;
//     // Returns the temperature in degrees F, K, R, or C (default if unspecified)
//     float readTemp(MAX31855k::units _u = C);
//     // Returns the cold junction temperature in ˚C
//     float readCJT(void);

//     // Pass a pin number to set as CS
//     void setCS(int pin);

//     MAX31855k(const uint8_t, const uint8_t _vcc = NONE,
//               const uint8_t _gnd = NONE, const bool _debug = 0);
//     ~MAX31855k() {} // User responsible 4 reassigning pins & stopping SPI
// };
