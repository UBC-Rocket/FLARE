/*
 * Radio Header
 *
 * @file    radio.h
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

#pragma once

/*Includes------------------------------------------------------------*/
#include <stdint.h>

#include "XBee.h"
#include "radio_interface.h"
#include "radio_queue.h"

#include "statemachine.h"

/*Constants------------------------------------------------------------*/
/* radio addressing */
constexpr uint64_t GND_STN_ADDR_MSB = 0x0013A200; // Ground Station - Body
constexpr uint64_t GND_STN_ADDR_LSB = 0x41678FC0;
constexpr uint32_t RADIO_BAUD_RATE = 921600;

class RadioController : public RadioControllerBase {
  public:
    /**
     * @brief Constructor.
     * @param MAX_QUEUED_BYTES Maximum number of subpacket data bytes to queue
     * before dropping the oldest subpackets.
     * @param SERIAL_RADIO Uninitialized HardwareSerial used for radio (e.g.
     * SerialRadio)
     */
    RadioController(Hal::Serial &serial_radio,
                    unsigned short const MAX_QUEUED_BYTES = 800,
                    uint8_t const MAX_PACKETS_PER_RX_LOOP = 8)
        : RadioControllerBase(MAX_QUEUED_BYTES),
          m_gnd_addr(XBeeAddress64(GND_STN_ADDR_MSB, GND_STN_ADDR_LSB)),
          M_MAX_PACKETS_PER_RX_LOOP(MAX_PACKETS_PER_RX_LOOP) {

        serial_radio.begin(RADIO_BAUD_RATE);
        while (!serial_radio)
            ;
        m_xbee.setSerial(serial_radio.getSerial());
        m_tx_packet.setAddress64(m_gnd_addr);
        m_tx_packet.setPayload(m_payload);
    }

    /**
     * @brief Meat of the action - listens for any incoming packets, then
     * transmits data and performs rocket actions as necessary.
     */
    void listenAndAct();

  private:
    XBee m_xbee;
    XBeeAddress64 m_gnd_addr;
    ZBTxRequest m_tx_packet;
    ZBRxResponse rx;

    uint8_t m_payload[RADIO_MAX_SUBPACKET_SIZE];

    uint8_t M_MAX_PACKETS_PER_RX_LOOP;
    void send();
};
