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

#include "HAL/time.h"
#include "XBee.h"
#include "ignitor_collection.h"
#include "radio_queue.h"
#include "sensor_collection.h"
#include "sensors/GPS.h"
#include "sensors/IMU.h"
#include "sensors/accelerometer.h"

/*Constants------------------------------------------------------------*/
/* radio addressing */
constexpr uint64_t GND_STN_ADDR_MSB = 0x0013A200; // Ground Station - Body
constexpr uint64_t GND_STN_ADDR_LSB = 0x41678FC0;
constexpr uint32_t RADIO_BAUD_RATE = 921600;

class RadioController {
  private:
    enum class Ids {
        status_ping = 0x00,
        message = 0x01,
        bulk_sensor = 0x30,
    };

  public:
    /**
     * @brief Constructor.
     * @param SERIAL_RADIO Uninitialized HardwareSerial used for radio (e.g.
     * SerialRadio)
     * @param MAX_QUEUED_BYTES Maximum number of subpacket data bytes to queue
     * before dropping the oldest subpackets.
     * @param MAX_PACKETS_PER_RX_LOOP Maximum number of packets to send every
     * time listenAndAct() is called.
     */
    RadioController(Hal::Serial &serial_radio,
                    unsigned short const MAX_QUEUED_BYTES = 800,
                    uint8_t const MAX_PACKETS_PER_RX_LOOP = 8)
        : gnd_addr_(XBeeAddress64(GND_STN_ADDR_MSB, GND_STN_ADDR_LSB)),
          tx_q_(MAX_QUEUED_BYTES),
          MAX_PACKETS_PER_RX_LOOP_(MAX_PACKETS_PER_RX_LOOP),
          WATCHDOG_SEND_INTERVAL_(3000), watchdog_last_send_(Hal::now_ms()) {

        serial_radio.begin(RADIO_BAUD_RATE);
        while (!serial_radio)
            ;
        xbee_.setSerial(serial_radio.getSerial());
        tx_packet_.setAddress64(gnd_addr_);
        tx_packet_.setPayload(payload_);
        sendMessage(Hal::millis(), "Radio initialized");
        send();
    }

    /**
     * @brief Meat of the action - listens for any incoming packets, then
     * transmits data and performs rocket actions as necessary.
     * @param act_upon Function-like object (e.g. lambda expression) that
     * accepts an unsigned char (representing the command) and returns void.
     */
    template <typename Func> void listenAndAct(Func act_upon);

    /**
     * @brief Add a subpacket to the queue to be sent. Note that this is a
     * rather low-level utility; there should also be a helper method for any
     * given subpacket that will build up the specific format this needs that
     * you should use instead. Note that due to move semantics you will likely
     * need to use std::move(dat).
     * @param dat A SubPktPtr (refer to typedef) containing the data.
     */
    void addSubpacket(SubPktPtr dat) { tx_q_.push(std::move(dat)); }

    /**
     * @brief Helper function to send status.
     * @param time Timestamp, in milliseconds
     * @param status Overall status of rocket
     * @param sensors Sensor collection, from which detailed status information
     * is extracted
     */
    void sendStatus(uint32_t time, RocketStatus status,
                    SensorCollection &sensors, IgnitorCollection &ignitors);

    /**
     * @brief Helper function to send bulk sensor data.
     * @param time Timestamp, in milliseconds
     * @param alt Altitude, in metres
     * @param xl Accelerometer sensor object
     * @param imu IMU sensor object
     * @param gps GPS sensor object
     * @param state_id Current rocket state, as integer ID
     */
    void sendBulkSensor(uint32_t time, float alt, Accelerometer &xl, IMU imu,
                        GPS gps, uint8_t state_id);

    /**
     * @brief Helper function to send message.
     * @param time Timestamp, in millisconds
     * @param str C-style string to send.
     */
    void sendMessage(const uint32_t time, const char *str);

  private:
    /**
     * @brief Fills in the ID and timestamp given a subpacket pointer.
     * Note that this method expects that the buffer has already been
     * appropreiately sized.
     * To avoid needing to deal with move semantics, this method expects a
     * raw pointer rather than a unique pointer, so you'll need to call the
     * get() method of SubPktPtr.
     */
    void setupIdTime(std::vector<uint8_t> *buf, Ids id, uint32_t time) {
        (*buf)[0] = static_cast<uint8_t>(id);
        std::memcpy(buf->data() + 1, &time, 4);
    }

    XBee xbee_;
    XBeeAddress64 gnd_addr_;
    ZBTxRequest tx_packet_;
    ZBRxResponse rx;

    RadioQueue tx_q_; // the [ueue] is silent :)
    uint8_t payload_[RADIO_MAX_SUBPACKET_SIZE];

    const uint8_t MAX_PACKETS_PER_RX_LOOP_;
    const Hal::ms WATCHDOG_SEND_INTERVAL_;
    Hal::t_point
        watchdog_last_send_; // Keeps track of last time sending was done.
    void send();

    // A bit of magic SFINAE from stack overflow, for listenAndAct
    // https://stackoverflow.com/a/16824239/10762409

    // Primary template, used for meaningful error message
    // Falls back to this if operator() is not present
    template <typename, typename T> struct can_receive_command {
        static_assert(std::integral_constant<T, false>::value,
                      "The passed in value for listenAndAct() must be callable "
                      "( has operator() )");
    };

    // specialization that does the checking
    template <typename C, typename Ret, typename... Args>
    struct can_receive_command<C, Ret(Args...)> {
      private:
        template <typename T>
        static constexpr auto check(T *) -> typename std::is_same<
            decltype(std::declval<T>().operator()(std::declval<Args>()...)),
            Ret      // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
            >::type; // attempt to call it and see if the return type is correct

        template <typename> static constexpr std::false_type check(...);

        typedef decltype(check<C>(0)) type;

      public:
        static constexpr bool value = type::value;
    };
};
