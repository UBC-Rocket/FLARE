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

#include "radio.h"

// #include "cameras.h"
// #include "gpio.h"
// #include "options.h"
// #include "satcom.h"
// #include "buzzer.h"
// #include "statemachine.h"

void RadioController::listenAndAct() {
    m_xbee.readPacket();
    uint8_t i = 0;
    while (i < M_MAX_PACKETS_PER_RX_LOOP &&
           (m_xbee.getResponse().isAvailable() ||
            m_xbee.getResponse().isError())) {
        // goes through all m_xbee packets in buffer

        if (m_xbee.getResponse()
                .isError()) { // TODO - figure out whether there's anything we
                              // should do about Xbee errors
            // #ifdef TESTING
            //     SerialUSB.println("m_Xbee error");
            // #endif
        } else if (m_xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
            // received command from m_xbee
            m_xbee.getResponse().getZBRxResponse(rx);
            uint8_t len = rx.getDataLength();
            uint8_t command;
            for (uint8_t j = 0; j < len; j++) {
                command = rx.getData()[j];
                // TODO - do something with the command
            }

        } else if (m_xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
            send();
        }

        m_xbee.readPacket();

        i++;
    }
}

void RadioController::send() {
    m_tx_packet.setPayloadLength(m_tx_q.fillPayload(m_payload));
    m_xbee.send(m_tx_packet);
}
