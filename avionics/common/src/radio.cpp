/*
 * Radio Source
 *
 * @file    radio.cpp
 * @author  UBC Rocket Avionics 2018/2019
 * @description  Implements radio send functions and prioritizing
 * sensor data to send over the radio.  This source utilizes the
 * XBee.cpp and .h files also included in this distro.
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
#include <algorithm> //for std::copy
#include <cstring>
#include <utility> //for std::move

#include "ignitor_collection.h"
#include "radio.h"
#include "sensor_collection.h"

void RadioController::send() {
    tx_packet_.setPayloadLength(tx_q_.fillPayload(payload_));
    xbee_.send(tx_packet_);
}

void RadioController::sendStatus(uint32_t time, RocketStatus status,
                                 SensorCollection &sensors,
                                 IgnitorCollection &ignitors) {
    SubPktPtr buf(new std::vector<uint8_t>);
    buf->resize(8);

    setupIdTime(buf.get(), Ids::status_ping, time);

    *(buf->data() + 5) = static_cast<uint8_t>(status);

    *(buf->data() + 6) = *sensors.getStatusBitfield();
    *(buf->data() + 7) = *(sensors.getStatusBitfield() + 1);
    *(buf->data() + 8) = *ignitors.getStatusBitfield();
    *(buf->data() + 9) = *(ignitors.getStatusBitfield() + 1);

    // TODO Include status of E-match components
    addSubpacket(std::move(buf));
}

void RadioController::sendBulkSensor(uint32_t time, float alt,
                                     Accelerometer &xl, IMU imu, GPS gps,
                                     uint8_t state_id) {
    SubPktPtr buf(new std::vector<uint8_t>);
    buf->resize(42);

    setupIdTime(buf.get(), Ids::bulk_sensor, time);

    // Altitude
    std::memcpy(buf->data() + 5, &alt, 4);

    // Accelerometer
    std::memcpy(buf->data() + 9, xl.getData(), 12);

    // IMU // TODO - check that these are the correct 3 floats to send for
    // orientation
    std::memcpy(buf->data() + 21, imu.getData(), 12);

    // GPS
    std::memcpy(buf->data() + 33, gps.getData(), 8);

    // State
    std::memcpy(buf->data() + 41, &state_id, 1);

    addSubpacket(std::move(buf));
}

void RadioController::sendMessage(const uint32_t time, const char *str) {
    SubPktPtr buf(new std::vector<uint8_t>);
    auto strlen = std::strlen(str);
    buf->resize(strlen + 6);
    setupIdTime(buf.get(), Ids::message, time);
    (*buf)[5] = strlen;
    std::memcpy(buf->data() + 6, str, strlen);
    addSubpacket(std::move(buf));
}
