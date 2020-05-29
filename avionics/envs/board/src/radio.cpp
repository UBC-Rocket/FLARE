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

#include "Adafruit_BNO055.h" //IMU
#include "GP20U7.h"          //GPS
#include "MS5803_01.h"       //barometer
#include "SparkFunTMP102.h"  //temp sensor
#include "SparkFun_LIS331.h" //accelerometer
// #include "battery.h"
#include "cameras.h"
#include "gpio.h"
#include "options.h"
// #include "satcom.h"
#include "buzzer.h"
#include "statemachine.h"

#include <Arduino.h>
#include <HardwareSerial.h>

/*Constants------------------------------------------------------------*/

/*
Radio commands recieved from the ground station.
*/
#define ARM 'r'
#define CAMERAS_ON 'C'
#define CAMERAS_OFF 'O'
#define RESET 'R'
#define PING 'p'
#define MAIN 'm'
#define DROGUE 'd'
#define STATUS 'S'
#define STARTUP_BUZZER 'B'
#define RECOVERY_BUZZER 'b'
#define GPS_PING 'g'
#define DO_NOTHING '\0'

/*
Status bit flags, for radio sending.
The first bit (0x01) is reserved for identifing Nosecone vs Body
*/
#define BAROMETER_BIT_FLAG 0x02
#define IMU_BIT_FLAG 0x04
#ifdef BODY
#define EMATCH_0_BIT_FLAG 0x08
#define EMATCH_1_BIT_FLAG 0x10
#endif
#ifdef NOSECONE
#define THERMOCOUPLE_BIT_FLAG 0x08
#define SATCOM_BIT_FLAG 0x10
#endif
#define FILE_BIT_FLAG 0x20

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
            uint8_t len = rx.getDataLength();
            m_xbee.getResponse().getZBRxResponse(rx);
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
