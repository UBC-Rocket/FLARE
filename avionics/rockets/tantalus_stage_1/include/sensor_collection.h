/*
 * Sensors Header
 *
 * @file    sensors.h
 * @author  UBC Rocket Avionics 2018/2019
 * @description   Main sensor initialization and polling file.
 * Implements sensor initialization and error checking as well as
 * sensor polling, sensor logging, and sending data over the radio.
 * Also performs rocket status-related work - startup buzzer, status report
 * generation, etc.
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

/*Includes------------------------------------------------------------*/
#include "HAL/time.h"
#include <array>
#include <functional> //for std::reference_wrapper

// #include "buzzer.h"
// #include "hw-interface.h"
// #include "options.h"
// #include "sensors-interface.h"
// #include "state_interface.h"
// #include "statemachine.h"
// #include "CSVwrite.h"

#include "HAL/port_impl.h"
#include "sensors/GPS.h"
#include "sensors/IMU.h"
#include "sensors/accelerometer.h"
#include "sensors/barometer.h"
#include "sensors/temperature.h"
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
    constexpr static std::size_t TEMP_INDEX = IMU_INDEX + IMU::dataLength();
    constexpr static std::size_t DATA_LENGTH =
        TEMP_INDEX + Temperature::dataLength();

    std::array<float, DATA_LENGTH> sensor_data;
    float *const &BEGIN = sensor_data.begin();

    Status status_;
    uint8_t status_bitfield_[3];

  public:
    constexpr static std::size_t NUM_SENSORS = 5;

    Barometer barometer;
    GPS gps;
    Accelerometer accelerometer;
    IMU imuSensor;
    Temperature temperature;

    SensorCollection()
        : barometer{BEGIN + BAROMETER_INDEX}, gps{Hal::SerialGPS,
                                                  BEGIN + GPS_INDEX},
          accelerometer{BEGIN + ACCEL_INDEX}, imuSensor{BEGIN + IMU_INDEX},
          temperature{BEGIN + TEMP_INDEX} {
        updateStatus();
    }

    void poll(Hal::t_point &timestamp) {
        timestamp = Hal::now_ms();
        barometer.readData();
        gps.readData();
        accelerometer.readData();
        imuSensor.readData();
        temperature.readData();
    }

    Status getStatus(bool refresh = false) {
        if (refresh) {
            updateStatus();
        }
        return status_;
    }

    uint8_t *getStatusBitfield() { return status_bitfield_; }
    void updateStatus() {
        status_ = Status::NOMINAL;
        status_bitfield_[0] = 0;
        if (barometer.getStatus() == SensorStatus::FAILURE) {
            raiseToStatus(status_, Status::CRITICAL_FAILURE);
            *status_bitfield_ |= 0x80;
        }

        if (gps.getStatus() == SensorStatus::FAILURE) {
            raiseToStatus(status_, Status::NONCRITICAL_FAILURE);
            *status_bitfield_ |= 0x40;
        }
        if (accelerometer.getStatus() == SensorStatus::FAILURE) {
            raiseToStatus(status_, Status::NONCRITICAL_FAILURE);
            *status_bitfield_ |= 0x20;
        }
        if (imuSensor.getStatus() == SensorStatus::FAILURE) {
            raiseToStatus(status_, Status::NONCRITICAL_FAILURE);
            *status_bitfield_ |= 0x10;
        }
        if (temperature.getStatus() == SensorStatus::FAILURE) {
            raiseToStatus(status_, Status::NONCRITICAL_FAILURE);
            *status_bitfield_ |= 0x08;
        }
    }

    const std::array<float, DATA_LENGTH> &getData() const {
        return sensor_data;
    }
};

/*Functions------------------------------------------------------------*/
typedef std::unique_ptr<SensorCollection> SensorCollectionPtr;
SensorCollectionPtr getSensors();

// void displayStatus(std::vector<std::reference_wrapper<ISensor>> &sensors,
//                    std::vector<std::reference_wrapper<IParachute>> &hardware,
//                    IBuzzer &buzzer);

// Status getStatus(std::vector<std::reference_wrapper<ISensor>> &sensors,
//                  std::vector<std::reference_wrapper<IParachute>> &hardware);

// void logData(unsigned long timestamp,
//  std::vector<std::reference_wrapper<ISensor>> &sensors,
//  StateId state, float altitude, float baseline_pressure);
