/*Includes------------------------------------------------------------*/
/*
 * Battery Check Source
 *
 * @file    battery.cpp
 * @author  UBC Rocket Avionics 2018/2019
 * @description   A battery voltage read function to check
 *  battery charge status.
 * 
 * Modified for use as sensor subclass
 *
 * @section LICENSE
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * Distributed as-is; no warranty is given.
 */

#include "sensors/battery_sensor.h"
#include "HAL/pin_util.h"


/*Constants------------------------------------------------------------*/
#define MINIMUM_BATTERY_VOLTAGE 10
#define LOW_BATTERY_VOLTAGE 11

#define R1 44170 // Nominal: 44200. Higher is larger range.
#define R2 15150 // Nominal: 16000. Lower is larger range.
// Maximum voltage = 3.3*(R1+R2)/R2

Battery::Battery(Pin batterySensorPin, float *const data)
    : SensorBase(data) {

/*init voltage sensor*/
#ifdef TESTING
    SerialUSB.println("Initializing battery voltage sensor");
#endif
    m_divider = static_cast<float>(R2) / (R1 + R2);
    m_batterySensorPin = batterySensorPin;
    // sets selected pin so it can be read from correctly
    Hal::pinMode(m_batterySensorPin, Hal::PinMode::INPUT);

    if (getBatteryVoltage() < LOW_BATTERY_VOLTAGE) {
        status = SensorStatus::FAILURE;
    } else {
        status = SensorStatus::NOMINAL;
    }
}

float sensor_range_map(int x, int in_min, int in_max, int out_min, int out_max) {
    return static_cast<float>((x - in_min) * (out_max - out_min)) / static_cast<float>((in_max - in_min) + out_min);
}

float Battery::getBatteryVoltage() {
#ifdef TESTING
    SerialUSB.println("Polling battery voltage sensor");
#endif
    int inputValue = Hal::analogRead(m_batterySensorPin);
    // map it to the range the analog out:
    // 3300mV is the highest voltage Teensy can read.
    float teensyVoltage = sensor_range_map(inputValue, 0, 1023, 0, 3300);
    // converts output value from mV to V and divides by voltage divider
    // value to calculate battery input voltage.
    float batteryVoltage = (teensyVoltage / m_divider) / 1000;
    return batteryVoltage;
}

void Battery::readData() {
    float batteryVoltage = getBatteryVoltage();
    data_[0] = batteryVoltage; 
}
