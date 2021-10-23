#ifndef SENSOR_COLLECTION_HPP_C433414CA96E4AB8A2065CE9C7E10AA3
#define SENSOR_COLLECTION_HPP_C433414CA96E4AB8A2065CE9C7E10AA3

/*
 * Sensor collection
 *
 * @file    sensor_collection.h
 * @author  UBC Rocket Avionics 2018-
 * @description   Deals with the sensor collection.
 *
 * @section LICENSE
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * Distributed as-is; no warranty is given.
 */

/*Includes------------------------------------------------------------*/
#include "HAL/time.h"
#include <array>
#include <functional> //for std::reference_wrapper
#include <memory>     //std::unique_ptr

#include "HAL/port_impl.h"
#include "sensors/GPS.h"
#include "sensors/IMU.h"
#include "sensors/accelerometer.h"
#include "sensors/barometer.h"
#include "sensors/temperature.h"
#include "sensors/battery_sensor.h"
#include "status.h"

/*Classes------------------------------------------------------------*/

class SensorCollection {
  private:
    constexpr static std::size_t BAROMETER_INDEX = 0;
    constexpr static std::size_t GPS_INDEX =
        BAROMETER_INDEX + Barometer::dataLength();
    constexpr static std::size_t ACCEL_INDEX = GPS_INDEX + GPS::dataLength();
    constexpr static std::size_t IMU_INDEX =
        ACCEL_INDEX + Accelerometer::dataLength();
    constexpr static std::size_t BATTERY_INDEX = IMU_INDEX + IMU::dataLength();
    constexpr static std::size_t TEMP_INDEX 
        = BATTERY_INDEX + Battery::dataLength();
    constexpr static std::size_t DATA_LENGTH =
        TEMP_INDEX + Temperature::dataLength();
    

    std::array<float, DATA_LENGTH> sensor_data;
    float *const BEGIN = sensor_data.begin();
 
    RocketStatus status_;
    uint8_t status_bitfield_[2];
    Hal::t_point last_poll_time_;

  public:
    constexpr static std::size_t NUM_SENSORS = 6;
    constexpr static char LOG_FILE_HEADER[] =
        "Pressure (mbar), Barom. Temp (C), GPS (lat), GPS (long), GPS (alt), "
        "Accel (x)(g), Accel (y)(g), Accel (z)(g), IMU (w), IMU(x), IMU(y), "
        "IMU(z), Voltage (V), Temperature (C)";

    Barometer barometer;
    GPS gps;
    Accelerometer accelerometer;
    IMU imuSensor;
    Battery battery;
    Temperature temperature;
    
    SensorCollection();

    /**
     * @brief Polls all the sensors
     * @return void
     */
    void poll();

    /**
     * @brief Return the recorded time when the sensors were last polled
     */
    Hal::t_point last_poll_time() { return last_poll_time_; }

    /**
     * @brief Gets sensor status
     * @param bool refresh If true, refreshes the status.
     * @return void
     */
    RocketStatus getStatus(bool refresh = false);
    const uint8_t *getStatusBitfield() const { return status_bitfield_; }
    void updateStatus();

    const std::array<float, DATA_LENGTH> &getData() const {
        return sensor_data;
    }
};

#endif
