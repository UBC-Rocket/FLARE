// /*
//  * MS5803 Barometer Header
//  *
//  * @file    MS5803_01.h
//  * @author  Luke Miller 2014 / UBC Rocket Avionics 2018/2019
//  * @description  An Arduino library for the Measurement Specialties MS5803
//  * family of pressure sensors. This library uses I2C to communicate with the
//  * MS5803 using the Wire library from Arduino.
//  *
//  * This library only works with the MS5803-01BA model sensor. It DOES NOT
//  * work with the other pressure-range models such as the MS5803-30BA or
//  * MS5803-02BA. Those models will return incorrect pressure and temperature
//  * readings if used with this library. See http://github.com/millerlp for
//  * libraries for the other models.
//  *
//  * This file was written by Luke Miller in 2014 and extended by the
//  * UBC Rocket Avionics Team in 2019.
//  *
//  * @section LICENSE
//  * GNU GPL v3
//  * Copyright (c) April 1 2014, Luke Miller
//  *
//  * No warranty is given or implied. You are responsible for verifying that
//  * the outputs are correct for your sensor. There are likely bugs in
//  * this code that could result in incorrect pressure readings, particularly
//  * due to variable overflows within some pressure ranges.
//  * DO NOT use this code in a situation that could result in harm to you or
//  * others because of incorrect pressure readings.
//  *
//  * UBC Rocket Avionics Team:
//  * This program is free software; you can redistribute it and/or
//  * modify it under the terms of the GNU General Public License as
//  * published by the Free Software Foundation; either version 2 of
//  * the License, or (at your option) any later version.
//  *
//  * Distributed as-is; no warranty is given.
//  */

// #pragma once

// #include <Arduino.h>

// class MS_5803 {
//   public:
//     // Constructor for the class.
//     // The argument is the desired oversampling resolution, which has
//     // values of 256, 512, 1024, 2048, 4096
//     MS_5803(uint16_t Resolution = 512);
//     // Initialize the sensor
//     boolean initializeMS_5803(boolean Verbose = true);
//     // Reset the sensor
//     bool resetSensor();
//     // Read the sensor
//     bool readSensor();
//     //*********************************************************************
//     // Additional methods to extract temperature, pressure (mbar), and the
//     // D1,D2 values after readSensor() has been called

//     // Return temperature in degrees Celsius.
//     float temperature() const { return tempC; }
//     // Return pressure in mbar.
//     float pressure() const { return mbar; }
//     //    // Return temperature in degress Fahrenheit.
//     //    float temperatureF() const		{return tempF;}
//     //    // Return pressure in psi (absolute)
//     //    float psia() const				{return psiAbs;}
//     //    // Return pressure in psi (gauge)
//     //    float psig() const				{return psiGauge;}
//     //    // Return pressure in inHg
//     //    float inHg() const				{return inHgPress;}
//     //    // Return pressure in mmHg
//     //    float mmHg() const				{return mmHgPress;}
//     // Return the D1 and D2 values, mostly for troubleshooting
//     unsigned long D1val() const { return D1; }
//     unsigned long D2val() const { return D2; }

//   private:
//     float mbar;  // Store pressure in mbar.
//     float tempC; // Store temperature in degrees Celsius

//     bool _transmitSuccess; // true normally, false if transmission failed at
//                            // some point
//     //    float tempF; // Store temperature in degrees Fahrenheit
//     //    float psiAbs; // Store pressure in pounds per square inch, absolute
//     //    float psiGauge; // Store gauge pressure in pounds per square inch
//     //    (psi) float inHgPress;	// Store pressure in inches of mercury float
//     //    mmHgPress;	// Store pressure in mm of mercury
//     unsigned long D1; // Store D1 value
//     unsigned long D2; // Store D2 value
//     int32_t mbarInt;  // pressure in mbar, initially as a signed long integer
//     // Check data integrity with CRC4
//     unsigned char MS_5803_CRC(unsigned int n_prom[]);
//     // Handles commands to the sensor.
//     unsigned long MS_5803_ADC(char commandADC);
//     // Oversampling resolution
//     uint16_t _Resolution;
// };

