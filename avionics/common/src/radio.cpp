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
#include <utility>   //for std::move

#include "ignitor_collection.h"
#include "radio.h"
#include "sensor_collection.h"

// #include "cameras.h"
// #include "gpio.h"
// #include "options.h"
// #include "satcom.h"
// #include "buzzer.h"

void RadioController::listenAndAct() {
    xbee_.readPacket();
    uint8_t i = 0;
    while (i < MAX_PACKETS_PER_RX_LOOP_ && (xbee_.getResponse().isAvailable() ||
                                            xbee_.getResponse().isError())) {
        // goes through all xbee_ packets in buffer

        if (xbee_.getResponse()
                .isError()) { // TODO - figure out whether there's anything we
                              // should do about Xbee errors
            // #ifdef TESTING
            //     SerialUSB.println("xbee_ error");
            // #endif
        } else if (xbee_.getResponse().getApiId() == ZB_RX_RESPONSE) {
            // received command from xbee_
            xbee_.getResponse().getZBRxResponse(rx);
            uint8_t len = rx.getDataLength();
            uint8_t command;
            for (uint8_t j = 0; j < len; j++) {
                command = rx.getData()[j];
                // TODO - do something with the command
            }

        } else if (xbee_.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
            send();
        }

        xbee_.readPacket();

        i++;
    }
}

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
