// /*
//  * MS5803 Barometer Source
//  *
//  * @file    MS5803_01.cpp
//  * @author  Luke Miller 2014 / UBC Rocket Avionics 2018/2019
//  * @description  An Arduino library for the Measurement Specialties MS5803 family
//  * of pressure sensors. This library uses I2C to communicate with the
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

// #include "MS5803_01.h"
// #include <i2c_t3.h>
// //#include <Wire.h>

// // For I2C, set the CSB Pin (pin 3) high for address 0x76, and pull low
// // for address 0x77. If you use 0x77, change the value on the line below:
// #define MS5803_I2C_ADDRESS  0x77    //0x76 // or 0x77

// #define CMD_RESET		0x1E	// ADC reset command
// #define CMD_ADC_READ	0x00	// ADC read command
// #define CMD_ADC_CONV	0x40	// ADC conversion command
// #define CMD_ADC_D1		0x00	// ADC D1 conversion
// #define CMD_ADC_D2		0x10	// ADC D2 conversion
// #define CMD_ADC_256		0x00	// ADC resolution=256
// #define CMD_ADC_512		0x02	// ADC resolution=512
// #define CMD_ADC_1024	0x04	// ADC resolution=1024
// #define CMD_ADC_2048	0x06	// ADC resolution=2048
// #define CMD_ADC_4096	0x08	// ADC resolution=4096


// // Create array to hold the 8 sensor calibration coefficients
// static unsigned int      sensorCoeffs[8]; // unsigned 16-bit integer (0-65535)

// // These three variables are used for the conversion steps
// // They should be signed 32-bit integer initially
// // i.e. signed long from -2147483648 to 2147483647
// static int32_t	dT = 0;
// static int32_t 	TEMP = 0;
// // These values need to be signed 64 bit integers
// // (long long = int64_t)
// static int64_t	Offset = 0;
// static int64_t	Sensitivity  = 0;
// static int64_t	T2 = 0;
// static int64_t	OFF2 = 0;
// static int64_t	Sens2 = 0;
// // bytes to hold the results from I2C communications with the sensor
// static byte HighByte;
// static byte MidByte;
// static byte LowByte;


// //-------------------------------------------------
// // Constructor
// MS_5803::MS_5803(uint16_t Resolution) {
// 	// The argument is the oversampling resolution, which may have values
// 	// of 256, 512, 1024, 2048, or 4096.
// 	_Resolution = Resolution;
// }

// //-------------------------------------------------
// boolean MS_5803::initializeMS_5803(boolean Verbose) {
//     //Wire.begin();
//     // Reset the sensor during startup
//     if (!resetSensor())
//         return false;

//     if (Verbose) {
//     	// Display the oversampling resolution or an error message
//     	if (_Resolution == 256 || _Resolution == 512 || _Resolution == 1024 || _Resolution == 2048 || _Resolution == 4096){
//         	Serial.print("Oversampling setting: ");
//         	Serial.println(_Resolution);
//     	} else {
// 			Serial.println("*******************************************");
// 			Serial.println("Error: specify a valid oversampling value");
// 			Serial.println("Choices are 256, 512, 1024, 2048, or 4096");
// 			Serial.println("*******************************************");
//     	}

//     }
// 	// Read sensor coefficients
//     for (int i = 0; i < 8; i++ ){
//     	// The PROM starts at address 0xA0
//     	Wire.beginTransmission(MS5803_I2C_ADDRESS);
//     	Wire.write(0xA0 + (i * 2));
//     	if(Wire.endTransmission()){ //transmission failed
//             return false;
//         }
//     	Wire.requestFrom(MS5803_I2C_ADDRESS, 2);

//         if(Wire.available() < 2)
//             return false;

//     	while(Wire.available()) {
//     		HighByte = Wire.read();
//     		LowByte = Wire.read();
//     	}
//     	sensorCoeffs[i] = (((unsigned int)HighByte << 8) + LowByte);
//     	if (Verbose){
// 			// Print out coefficients
// 			Serial.print("C");
// 			Serial.print(i);
// 			Serial.print(" = ");
// 			Serial.println(sensorCoeffs[i]);
// 			delay(10);
//     	}
//     }
//     // The last 4 bits of the 7th coefficient form a CRC error checking code.
//     unsigned char p_crc = sensorCoeffs[7];
//     p_crc = p_crc & 0x0F;
//     // Use a function to calculate the CRC value
//     unsigned char n_crc = MS_5803_CRC(sensorCoeffs);

//     if (Verbose) {
// 		Serial.print("p_crc: ");
// 		Serial.println(p_crc);
// 		Serial.print("n_crc: ");
// 		Serial.println(n_crc);
//     }
//     // If the CRC value doesn't match the sensor's CRC value, then the
//     // connection can't be trusted. Check your wiring.
//     if (p_crc != n_crc) {
//         return false;
//     }
//     //Otherwise, return true when everything checks out OK.
//     return true;
// }

// //------------------------------------------------------------------
// /*
//  * @brief Reads the sensor and stores the values.
//  * @return Returns true if successfully read, false otherwise. Note that
//  *      reading the temperature and pressure is still possible after this function
//  *      fails; those values will simply not be updated.
//  * */
// bool MS_5803::readSensor() {
//     // D1 and D2 need to be unsigned 32-bit integers (long 0-4294967295)
//     static uint32_t     D1 = 0;    // Store uncompensated pressure value
//     static uint32_t     D2 = 0;    // Store uncompensated temperature value

// 	// Choose from CMD_ADC_256, 512, 1024, 2048, 4096 for mbar resolutions
// 	// of 1, 0.6, 0.4, 0.3, 0.2 respectively. Higher resolutions take longer
// 	// to read.
//     _transmitSuccess = true;
// 	if (_Resolution == 256){
// 		D1 = MS_5803_ADC(CMD_ADC_D1 + CMD_ADC_256); // read raw pressure
// 		D2 = MS_5803_ADC(CMD_ADC_D2 + CMD_ADC_256); // read raw temperature
// 	} else if (_Resolution == 512) {
// 		D1 = MS_5803_ADC(CMD_ADC_D1 + CMD_ADC_512); // read raw pressure
// 		D2 = MS_5803_ADC(CMD_ADC_D2 + CMD_ADC_512); // read raw temperature
// 	} else if (_Resolution == 1024) {
// 		D1 = MS_5803_ADC(CMD_ADC_D1 + CMD_ADC_1024); // read raw pressure
// 		D2 = MS_5803_ADC(CMD_ADC_D2 + CMD_ADC_1024); // read raw temperature
// 	} else if (_Resolution == 2048) {
// 		D1 = MS_5803_ADC(CMD_ADC_D1 + CMD_ADC_2048); // read raw pressure
// 		D2 = MS_5803_ADC(CMD_ADC_D2 + CMD_ADC_2048); // read raw temperature
// 	} else if (_Resolution == 4096) {
// 		D1 = MS_5803_ADC(CMD_ADC_D1 + CMD_ADC_4096); // read raw pressure
// 		D2 = MS_5803_ADC(CMD_ADC_D2 + CMD_ADC_4096); // read raw temperature
// 	}

//     if (!_transmitSuccess)
//         return false;
//     // Calculate 1st order temperature, dT is a long signed integer
// 	// D2 is originally cast as an uint32_t, but can fit in a int32_t, so we'll
// 	// cast both parts of the equation below as signed values so that we can
// 	// get a negative answer if needed
//     dT = (int32_t)D2 - ( (int32_t)sensorCoeffs[5] * 256 );
//     // Use integer division to calculate TEMP. It is necessary to cast
//     // one of the operands as a signed 64-bit integer (int64_t) so there's no
//     // rollover issues in the numerator.
//     TEMP = 2000 + ((int64_t)dT * sensorCoeffs[6]) / 8388608LL;
//     // Recast TEMP as a signed 32-bit integer
//     TEMP = (int32_t)TEMP;


//     // All operations from here down are done as integer math until we make
//     // the final calculation of pressure in mbar.


//     // Do 2nd order temperature compensation (see pg 9 of MS5803 data sheet)
//     // I have tried to insert the fixed values wherever possible
//     // (i.e. 2^31 is hard coded as 2147483648).
//     if (TEMP < 2000) {
// 		// For 1 bar model
// 		T2 = ((int64_t)dT * dT) / 2147483648ULL ; // 2^31 = 2147483648
// 		T2 = (int32_t)T2; // recast as signed 32bit integer
// 		OFF2 = 3 * ((TEMP-2000) * (TEMP-2000));
// 		Sens2 = 7 * ((TEMP-2000)*(TEMP-2000)) / 8 ;
//     } else { // if TEMP is > 2000 (20.0C)
//     	// For 1 bar model
// 		T2 = 0;
// 		OFF2 = 0;
// 		Sens2 = 0;
// 		if (TEMP > 4500) {
// 			// Extra adjustment for high temps, only needed for 1 bar model
// 			Sens2 = Sens2 - ((TEMP-4500)*(TEMP-4500)) / 8;
// 		}
//     }

//     // Additional compensation for very low temperatures (< -15C)
//     if (TEMP < -1500) {
// 		// For 1 bar model
// 		// Leave OFF2 alone in this case
// 		Sens2 = Sens2 + 2 * ((TEMP+1500)*(TEMP+1500));
//     }

//     // Calculate initial Offset and Sensitivity
//     // Notice lots of casts to int64_t to ensure that the
//     // multiplication operations don't overflow the original 16 bit and 32 bit
//     // integers

// 	// For 1 bar sensor
// 	Offset = (int64_t)sensorCoeffs[2] * 65536 + (sensorCoeffs[4] * (int64_t)dT) / 128;
// 	Sensitivity = (int64_t)sensorCoeffs[1] * 32768 + (sensorCoeffs[3] * (int64_t)dT) / 256;

//     // Adjust TEMP, Offset, Sensitivity values based on the 2nd order
//     // temperature correction above.
//     TEMP = TEMP - T2; // both should be int32_t
//     Offset = Offset - OFF2; // both should be int64_t
//     Sensitivity = Sensitivity - Sens2; // both should be int64_t

//     // Final compensated pressure calculation. We first calculate the pressure
//     // as a signed 32-bit integer (mbarInt), then convert that value to a
//     // float (mbar).

// 	// For 1 bar sensor
// 	mbarInt = ((D1 * Sensitivity) / 2097152 - Offset) / 32768;
// 	mbar = (float)mbarInt / 100;

// 	// Calculate the human-readable temperature in Celsius
// 	tempC  = (float)TEMP / 100;

//     // Start other temperature conversions by converting mbar to psi absolute
// //    psiAbs = mbar * 0.0145038;
// //    // Convert psi absolute to inches of mercury
// //    inHgPress = psiAbs * 2.03625;
// //    // Convert psi absolute to gauge pressure
// //    psiGauge = psiAbs - 14.7;
// //    // Convert mbar to mm Hg
// //    mmHgPress = mbar * 0.7500617;
// //    // Convert temperature to Fahrenheit
// //    tempF = (tempC * 1.8) + 32;

//     return true;
// }

// //------------------------------------------------------------------
// // Function to check the CRC value provided by the sensor against the
// // calculated CRC value from the rest of the coefficients.
// // Based on code from Measurement Specialties application note AN520
// // http://www.meas-spec.com/downloads/C-Code_Example_for_MS56xx,_MS57xx_%28except_analog_sensor%29_and_MS58xx_Series_Pressure_Sensors.pdf
// unsigned char MS_5803::MS_5803_CRC(unsigned int n_prom[]) {
//     int cnt;				// simple counter
//     unsigned int n_rem;		// crc remainder
//     unsigned int crc_read;	// original value of the CRC
//     unsigned char  n_bit;
//     n_rem = 0x00;
//     crc_read = sensorCoeffs[7];		// save read CRC
//     sensorCoeffs[7] = (0xFF00 & (sensorCoeffs[7])); // CRC byte replaced with 0
//     for (cnt = 0; cnt < 16; cnt++)
//     { // choose LSB or MSB
//         if (cnt%2 == 1) {
//         	n_rem ^= (unsigned short)((sensorCoeffs[cnt>>1]) & 0x00FF);
//         }
//         else {
//         	n_rem ^= (unsigned short)(sensorCoeffs[cnt>>1] >> 8);
//         }
//         for (n_bit = 8; n_bit > 0; n_bit--)
//         {
//             if (n_rem & (0x8000))
//             {
//                 n_rem = (n_rem << 1) ^ 0x3000;
//             }
//             else {
//                 n_rem = (n_rem << 1);
//             }
//         }
//     }
//     n_rem = (0x000F & (n_rem >> 12));// // final 4-bit reminder is CRC code
//     sensorCoeffs[7] = crc_read; // restore the crc_read to its original place
//     // Return n_rem so it can be compared to the sensor's CRC value
//     return (n_rem ^ 0x00);
// }

// //-----------------------------------------------------------------
// // Send commands and read the temperature and pressure from the sensor
// unsigned long MS_5803::MS_5803_ADC(char commandADC) {
// 	// D1 and D2 will come back as 24-bit values, and so they must be stored in
// 	// a long integer on 8-bit Arduinos.
//     long result = 0;
//     // Send the command to do the ADC conversion on the chip
// 	Wire.beginTransmission(MS5803_I2C_ADDRESS);
//     Wire.write(CMD_ADC_CONV + commandADC);
//     Wire.endTransmission();
//     // Wait a specified period of time for the ADC conversion to happen
//     // See table on page 1 of the MS5803 data sheet showing response times of
//     // 0.5, 1.1, 2.1, 4.1, 8.22 ms for each accuracy level.
//     switch (commandADC & 0x0F)
//     {
//         case CMD_ADC_256 :
//             delayMicroseconds(650); // 1 ms
//             break;
//         case CMD_ADC_512 :
//             delayMicroseconds(1220); // 3 ms
//             break;
//         case CMD_ADC_1024:
//             delayMicroseconds(2330);
//             break;
//         case CMD_ADC_2048:
//             delayMicroseconds(4590);
//             break;
//         case CMD_ADC_4096:
//             delayMicroseconds(9090);
//             break;
//     }
//     // Now send the read command to the MS5803
//     Wire.beginTransmission(MS5803_I2C_ADDRESS);
//     Wire.write((byte)CMD_ADC_READ);
//     if(Wire.endTransmission())
//         _transmitSuccess = false;
//     // Then request the results. This should be a 24-bit result (3 bytes)
//     if(Wire.requestFrom(MS5803_I2C_ADDRESS, 3) != 3)
//         _transmitSuccess = false;

//     if(Wire.available() < 3)
//         _transmitSuccess = false;

//     while(Wire.available()) {
//     	HighByte = Wire.read();
//     	MidByte = Wire.read();
//     	LowByte = Wire.read();
//     }
//     // Combine the bytes into one integer
//     result = ((long)HighByte << 16) + ((long)MidByte << 8) + (long)LowByte;
//     return result;
// }

// //----------------------------------------------------------------
// /*
//  * @ brief Resets the semnsor
//  * @ return boolean - true if suceeded, false if otherwise
//  */
// bool MS_5803::resetSensor() {
//     	Wire.beginTransmission(MS5803_I2C_ADDRESS);
//         Wire.write(CMD_RESET);
//         if(Wire.endTransmission())
//             return false;
//     	delay(10);
//         return true;
// }
