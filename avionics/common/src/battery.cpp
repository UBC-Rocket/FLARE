/*
 * Battery Check Source
 *
 * @file    battery.cpp
 * @author  UBC Rocket Avionics 2018/2019
 * @description   A battery voltage read function to check
 *  battery charge status.
 *
 * @section LICENSE
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * Distributed as-is; no warranty is given.
 */

#include "battery.h"
#include "sensors.h"
#include "Arduino.h"

/*Constants------------------------------------------------------------*/
#define MINIMUM_BATTERY_VOLTAGE 10
#define LOW_BATTERY_VOLTAGE 11

#define R1 44170  //Nominal: 44200. Higher is larger range.
#define R2 15150  //Nominal: 16000. Lower is larger range.
//Maximum voltage = 3.3*(R1+R2)/R2



Battery::Battery(byte batterySensorPin)
{
    m_divider = static_cast<float>(R2) /(R1 + R2);
    m_batterySensorPin = batterySensorPin;
}


float Battery::getVoltage()
{
    float inputValue = analogRead(m_batterySensorPin);
    // map it to the range the analog out:
    // 3300mV is the highest voltage Teensy can read.
    float teensyVoltage = map(inputValue, 0, 1023, 0, 3300);
    // converts output value from mV to V and divides by voltage divider
    // value to calculate battery input voltage.
    float batteryVoltage = (teensyVoltage / m_divider) / 1000;
    return batteryVoltage;
}

// OverallError Battery::getStatus()
// {
//     float voltage = getVoltage();
//     if(voltage < MINIMUM_BATTERY_VOLTAGE)
//         return CRITICAL_FAILURE;
//     else if (voltage < LOW_BATTERY_VOLTAGE)
//         return NONCRITICAL_FAILURE;
//     else
//         return NOMINAL;
// }
