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

#ifndef __RADIO_H__
#define __RADIO_H__

/*Includes------------------------------------------------------------*/
#include <Arduino.h>
#include <stdint.h>
#include <deque> // for std::deque
#include <memory> // for std::unique_ptr


#include "statemachine.h"
#include "sensors.h"
#include "options.h"
#include "XBee.h"

/*Constants------------------------------------------------------------*/
/* radio addressing */
constexpr uint64_t GND_STN_ADDR_MSB = 0x0013A200; //Ground Station - Body
constexpr uint64_t GND_STN_ADDR_LSB = 0x41678FC0;
constexpr uint32_t RADIO_BAUD_RATE = 921600;
constexpr unsigned short RADIO_MAX_SUBPACKET_SIZE = 255;
/*Classes--------------------------------------------------------------*/

typedef std::unique_ptr<std::vector<int>> SubPktPtr;

class RadioQueue {
public:
    RadioQueue(unsigned short const MAX_BYTES) : M_MAX_BYTES(MAX_BYTES) {}

    /**
     * @brief Push a subpacket to the queue. If the pointer is a null pointer or the size is too large, it will silently fail.
     * @param subpacket SubPktPtr to data.
     * */
    void push(SubPktPtr subpacket);

    /**
     * @brief Returns the next SubPktPtr in the queue (i.e. the oldest element), popping that element. If SubPktPtr is empty, behaviour is undefined.
     */
    SubPktPtr popFront();

    /**
     * @brief Test whether container is empty.
     */
    bool empty() const {return m_subpacket_q.empty();}

    /**
     * @brief Get number of bytes of subpacket data queued up. Note that this is not the same as the number of subpackets that are queued.
     * */
    unsigned short getByteCount() const {return m_byte_count;}

    /**
     * @brief Fills payload pointer as full as possible (i.e. at most RADIO_MAX_SUBPACKET_SIZE bytes)
     * @param payload Pointer to payload array.
     * @return Size of payload data filled in.
     */
    uint8_t fillPayload(uint8_t *payload);

private:
    std::deque<SubPktPtr> m_subpacket_q; //subpacket_queue, but the ueue is silent
    unsigned short m_byte_count = 0;
    unsigned short const M_MAX_BYTES;

    /**
     * @brief Dump entire queue into payload. Must check that payload is large enough and queue is small enough that this works (i.e. m_byte_count < payload array size); otherwise this is undefined behaviour
     * @return Size of payload data filled in (= m_byte_count before this starts).
     */
    uint8_t dumpAllIntoPayload(uint8_t *payload);

    /**
     * @brief Dispenses at most RADIO_MAX_SUBPACKET_SIZE (255) bytes worth of subpackets into payload.
     * @return Size of payload data filled in.
     */
    uint8_t dispenseIntoPayload(uint8_t *payload);
};

class RadioController {
public:
    /**
     * @brief Constructor.
     * @param MAX_QUEUED_BYTES Maximum number of subpacket data bytes to queue before dropping the oldest subpackets.
     * @param SERIAL_RADIO Uninitialized HardwareSerial used for radio (e.g. SerialRadio)
     */
    RadioController(HardwareSerial &serial_radio,
            unsigned short const MAX_QUEUED_BYTES = 800) :
        m_gnd_addr(XBeeAddress64(GND_STN_ADDR_MSB, GND_STN_ADDR_LSB)),
        m_tx_q(MAX_QUEUED_BYTES) {

        serial_radio.begin(RADIO_BAUD_RATE);
        while (!SerialRadio);
        m_xbee.setSerial(serial_radio);
        m_tx_packet.setAddress64(m_gnd_addr);
        m_tx_packet.setPayload(m_payload);
    }

    /**
     * Add a subpacket to the queue to be sent. Note that this is a rather low-level utility; there should also be a helper method for any given subpacket that will build up the specific format this needs that you should use instead.
     * @param dat A SubPktPtr (refer to typedef) containing the data.
     */
    void addSubpacket(SubPktPtr dat);

    /**
     * @brief Meat of the action - listens for any incoming packets, then transmits data and performs rocket actions as necessary.
     */
    void listenAndAct();

private:
    XBee m_xbee;
    XBeeAddress64 m_gnd_addr;
    ZBTxRequest m_tx_packet;

    RadioQueue m_tx_q;

    uint8_t m_payload[RADIO_MAX_SUBPACKET_SIZE];

    void send();
};


/*Functions------------------------------------------------------------*/
void sendRadioBody(XBee* radio, ZBTxRequest* txPacket, float*, FlightStates
        state, float *altitude, uint32_t*);
void sendRadioNosecone(XBee* radio, ZBTxRequest* txPacket, float* GPS_data,
        float bar_data[], float acc_data[], float *temp_sensor_data, float IMU_data[]);

void resolveRadioRx(XBee* radio, ZBTxRequest* txPacket, float* GPS_data,
        FlightStates *state, Status *status);
void radioStatus(XBee* radio, ZBTxRequest* txPacket, Status *status);
void doCommand(char command, float GPS_data[], FlightStates *state,
        Status *status, XBee* radio, ZBTxRequest* txPacket);
void sendMessage(XBee* radio, ZBTxRequest* txPacket, String* msg);
#endif
