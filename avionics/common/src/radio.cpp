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

#define RADIO_CONFIG_PACKET_VERSION_STR "TestVersionString" // TODO :  Grab from git and define in build system

void RadioController::send() {
    if (!tx_q_.empty()) {
        tx_packet_.setPayloadLength(tx_q_.fillPayload(payload_));
        xbee_.send(tx_packet_);
    }
}

void RadioController::sendStatus(uint32_t time, RocketStatus status,
                                 SensorCollection &sensors,
                                 IgnitorCollection &ignitors) {
    PacketBuffWriter buf;
    addIdTime(buf, Ids::status_ping, time);

    buf.write(&status, 1);

    buf.write(sensors.getStatusBitfield(), 1);
    buf.write(sensors.getStatusBitfield() + 1, 1);
    buf.write(ignitors.getStatusBitfield(), 1);
    buf.write(ignitors.getStatusBitfield() + 1, 1);

    addSubpacket(std::move(buf.packet));
}

void RadioController::sendBulkSensor(uint32_t time, float alt,
                                     Accelerometer &xl, IMU &imu, GPS &gps,
                                     uint8_t state_id) {
    PacketBuffWriter buf;
    addIdTime(buf, Ids::bulk_sensor, time);

    // Altitude
    buf.write(&alt, 4);

    // Accelerometer
    buf.write(xl.getData(), 12);

    // IMU // TODO - check that these are the correct 3 floats to send for
    // orientation
    buf.write(imu.getData(), 12);

    // GPS
    buf.write(gps.getData(), 8);

    // State
    buf.write(&state_id, 1);

    addSubpacket(std::move(buf.packet));
}

void RadioController::sendMessage(const uint32_t time, const char *str) {
    PacketBuffWriter buf;
    addIdTime(buf, Ids::message, time);

    uint8_t strlen = std::strlen(str);
    buf.write(strlen);
    buf.write(str, strlen);

    addSubpacket(std::move(buf.packet));
}

void RadioController::sendGPS(const uint32_t time, GPS &gps) {
    PacketBuffWriter buf;
    addIdTime(buf, Ids::gps, time);

    buf.write(gps.getData(), 12);

    addSubpacket(std::move(buf.packet));
}

void RadioController::sendSingleSensor(const uint32_t time, uint8_t id,
                                       float data) {
    PacketBuffWriter buf;
    buf.write(id);
    buf.write(&time, sizeof(time));

    buf.write(&data, 4);
    
    addSubpacket(std::move(buf.packet));
}

void RadioController::sendState(const uint32_t time, uint8_t state_id) {
    PacketBuffWriter buf;

    buf.write(0x1D); // id
    buf.write(&time, 4);
    buf.write(0);
    buf.write(0);
    buf.write(0);
    buf.write(state_id);

    addSubpacket(std::move(buf.packet));
}

void RadioController::sendConfig(const uint32_t time) {
    PacketBuffWriter buf;
    addIdTime(buf, Ids::config, time);

    // Defined in CMakeLists/platformio.ini
    buf.write(RADIO_CONFIG_PACKET_SIM_ACTIVE);
    buf.write(RADIO_CONFIG_PACKET_ROCKET_ID);

    constexpr size_t len = sizeof(RADIO_CONFIG_PACKET_VERSION_STR)-1; // -1 because null terminated string
    static_assert(len <= 40, "RADIO_CONFIG_PACKET_VERSION_STR longer than 40 characters!");
    uint8_t version_bytes[40] = {0};
    std::memcpy(version_bytes, RADIO_CONFIG_PACKET_VERSION_STR, len);

    buf.write(version_bytes, sizeof(version_bytes));
    
    addSubpacket(std::move(buf.packet));
}
