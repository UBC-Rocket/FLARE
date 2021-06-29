/*
 * Battery Check Header
 *
 * @file    battery.h
 * @author  UBC Rocket Avionics 2018/2019
 * @description   A battery voltage read function to check
 *  battery charge status.
 * 
 * Modified to implement battery check as a sensor class
 *
 * @section LICENSE
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * Distributed as-is; no warranty is given.
 */

#pragma once

#include "gpio.h"
#include "sensors_interface.h"

class Battery : public SensorBase<BATTERY_DATA_LENGTH>{
  public: // public functions
    // Constructor. batterySensorPin is the pin for battery sensor
    // (i.e. output of the voltage divider)
    Battery(Pin batterySensorPin, float *const data);

    /**
     * @brief  Gets the battery voltage level. Maximum voltage readable
     *          is 11.0 volts; to adjust, change the resistors used
     *          in the voltage divider.
     * @param  None
     * @return float batteryVoltage - voltage of the battery, in volts.
     */
    float readData();

  private:
    float m_divider;
    Pin m_batterySensorPin;
};
