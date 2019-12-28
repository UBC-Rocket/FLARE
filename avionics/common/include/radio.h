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
#include <queue> // for std::queue
#include <memory> // for std::unique_ptr


#include "statemachine.h"
#include "sensors.h"
#include "options.h"
#include "XBee.h"

/*Constants------------------------------------------------------------*/
/* radio addressing */
#define GND_STN_ADDR_MSB 0x0013A200 //Ground Station - Body
#define GND_STN_ADDR_LSB 0x41678FC0

/*Classes--------------------------------------------------------------*/

typedef std::unique_ptr<std::vector<int>> SubPktPtr;

class RadioQueue {
public:
    RadioQueue(unsigned short const MAX_BYTES = 800) : M_MAX_BYTES(MAX_BYTES) {}

    /**
     * @brief Push a subpacket to the queue. If the pointer is a null pointer or the size is too large, it will silently fail.
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

private:
    std::queue<SubPktPtr> m_subpacket_q; //subpacket_queue, but the ueue is silent
    unsigned short m_byte_count = 0;
    unsigned short const M_MAX_BYTES;
    static constexpr unsigned short MAX_SUBPACKET_SIZE = 255;
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
