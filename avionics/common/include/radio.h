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
#include <cstring>
#include <stdint.h>
#include <type_traits>
#include <utility>

#include "HAL/time.h"
#include "ignitor_collection.h"
#include "radio_queue.h"
#include "sensor_collection.h"
#include "sensors/GPS.h"
#include "sensors/IMU.h"
#include "sensors/accelerometer.h"

class PacketBuffWriter {
  public:
    PacketBuffWriter() : packet(new std::vector<uint8_t>) {
        packet->reserve(255); // To avoid overhead of resizing multiple times
    }

    void write(void const *data, size_t size) {
        if (!packet)
            return;

        size_t old_size = packet->size();
        packet->resize(old_size + size);
        std::memcpy(packet->data() + old_size, data, size);
    }

    void write(uint8_t byte) { write(&byte, 1); }

    SubPktPtr packet;
};

class Radio {
  private:
    enum class Ids {
        status_ping = 0x00,
        message = 0x01,
        config = 0x03,
        gps = 0x04,
        bulk_sensor = 0x30,
    };

    constexpr static Hal::ms WATCHDOG_SEND_INTERVAL{3000};
    constexpr static uint16_t STOP_PARSE_FLAG = 1 << 8;
    constexpr static uint16_t CAN_SEND_FLAG = 1 << 9;

  public:
    // type RadioMembers is public for technical reasons but cannot be used
    // (especially since no definition is given)
    class RadioMembers;

    typedef uint8_t command_t;
    // /**
    //  * @brief Constructor.
    //  * @param SERIAL_RADIO Uninitialized HardwareSerial used for radio
    //  (e.g.
    //  * SerialRadio)
    //  * @param MAX_QUEUED_BYTES Maximum number of subpacket data bytes to
    //  queue
    //  * before dropping the oldest subpackets.
    //  * @param MAX_PACKETS_PER_RX_LOOP Maximum number of packets to send
    //  every
    //  * time forwardCommand() is called.
    //  */
    // RadioController(Hal::Serial &serial_radio,
    //                 unsigned short const MAX_QUEUED_BYTES = 800,
    //                 uint8_t const MAX_PACKETS_PER_RX_LOOP = 8)
    //     : gnd_addr_(XBeeAddress64(GND_STN_ADDR_MSB, GND_STN_ADDR_LSB)),
    //       tx_q_(MAX_QUEUED_BYTES),
    //       MAX_PACKETS_PER_RX_LOOP_(MAX_PACKETS_PER_RX_LOOP),
    //       WATCHDOG_SEND_INTERVAL_(3000),
    //       watchdog_last_send(Hal::now_ms()) {

    //     serial_radio.begin(RADIO_BAUD_RATE);
    //     while (!serial_radio)
    //         ;
    //     xbee_.setSerial(serial_radio.getSerial());
    //     tx_packet_.setAddress64(gnd_addr_);
    //     tx_packet_.setPayload(payload_);
    //     sendMessage(Hal::millis(), "Radio initialized");
    //     send();
    // }

    /**
     * @brief Meat of the action - listens for any incoming packets, then
     * transmits data and performs rocket actions as necessary.
     * @param command_receiver Object with method run_command that
     * accepts an unsigned char (representing the command) and returns void.
     */
    template <typename Actor>
    static void forwardCommand(Actor &command_receiver) {
        static_assert(
            can_receive_command<Actor>::value,
            "act_upon does not have the expected signature void(uint8_t)");

        // Keeps track of last time sending was done.
        static Hal::t_point watchdog_last_send = Hal::now_ms();

        bool can_send = false;
        read_count_ = 0; // resetting
        uint16_t result;

        while (true) {
            command_t *command_data;
            command_t command_len;
            result = readPacket(command_data, command_len);
            if (result & STOP_PARSE_FLAG) {
                break;
            }
            for (int i = 0; i < command_len; ++i) {
                command_receiver.run_command(command_data[i]);
            }
        }
        //
        // while (read_count < MAX_PACKETS_PER_RX_LOOP_) {
        //     xbee_.readPacket();
        //     if (!(xbee_.getResponse().isAvailable() ||
        //           xbee_.getResponse().isError())) {
        //         break;
        //     }

        //     if (xbee_.getResponse().isError()) {
        //         // TODO - figure out whether there's anything
        //         // we should do about Xbee errors
        //         // #ifdef TESTING
        //         //     SerialUSB.println("xbee_ error");
        //         // #endif
        //     } else if (xbee_.getResponse().getApiId() ==
        //                ZB_TX_STATUS_RESPONSE) {
        //         can_send = true;
        //         // If we get 2 responses in a row, implies previously we sent
        //         an
        //         // extra one, so we shouldn't respond twice again.
        //     } else if (xbee_.getResponse().getApiId() == ZB_RX_RESPONSE) {
        //         // received command from xbee_
        //         xbee_.getResponse().getZBRxResponse(rx);
        //         uint8_t len = rx.getDataLength();
        //         uint8_t command;
        //         for (int j = 0; j < len; j++) {
        //             command = rx.getData()[j];
        //             command_receiver.run_command(command);
        //         }
        //     }

        //     read_count++;
        // }
        if (Hal::now_ms() - watchdog_last_send > WATCHDOG_SEND_INTERVAL) {
            can_send = true;
        }
        if (can_send) {
            can_send = false;
            watchdog_last_send = Hal::now_ms();
            send();
        }
    }

    /**
     * @brief Add a subpacket to the queue to be sent. Note that this is a
     * rather low-level utility; there should also be a helper method for any
     * given subpacket that will build up the specific format this needs that
     * you should use instead. Note that due to move semantics you will likely
     * need to use std::move(dat).
     * @param dat A SubPktPtr (refer to typedef) containing the data.
     */
    static void addSubpacket(SubPktPtr dat);

    /**
     * @brief Helper function to send status.
     * @param time Timestamp, in milliseconds
     * @param status Overall status of rocket
     * @param sensors Sensor collection, from which detailed status information
     * is extracted
     */
    static void sendStatus(uint32_t time, RocketStatus status,
                           SensorCollection &sensors,
                           IgnitorCollection &ignitors);

    /**
     * @brief Helper function to send bulk sensor data.
     * @param time Timestamp, in milliseconds
     * @param alt Altitude, in metres
     * @param xl Accelerometer sensor object
     * @param imu IMU sensor object
     * @param gps GPS sensor object
     * @param state_id Current rocket state, as integer ID
     */
    static void sendBulkSensor(uint32_t time, float alt, Accelerometer &xl,
                               IMU &imu, GPS &gps, uint8_t state_id);

    /**
     * @brief Helper function to send message.
     * @param time Timestamp, in millisconds
     * @param str C-style string to send.
     */
    static void sendMessage(const uint32_t time, const char *str);

    static void sendGPS(const uint32_t time, GPS &gps);

    /**
     * @brief Helper function to send single sensor. Logic of identifying sensor
     * not included - this is only really useful as an organizer.
     * @param time Timestamp, in milliseconds
     * @param id ID of the sensor to send - refer to specs on Confluence
     * @param data to be sent
     */
    static void sendSingleSensor(const uint32_t time, uint8_t id, float data);

    /**
     * @brief Helper function to send state. Falls under the spec for single
     * sensor, but for type reasons implemented as a seperate method.
     * @param time Timestamp, in milliseconds
     * @param state_id Current rocket state, as integer ID
     */
    static void sendState(const uint32_t time, uint8_t state_id);

    static void sendConfig(const uint32_t time);

  private:
    /**
     * @brief How many packets have been read in forwardCommand; primarily used
     * by readPacket()
     */
    static int read_count_;

    /**
     * @brief Helper function for forwardCommand, primarily around to
     * increase separation between interface (header) and implementation
     * (cpp)
     * @param command_dat_out Gets overwritten with a pointer to commands
     * received.
     * @param command_len_out Gets overwritten with the length of commands
     * received.
     * @return Bitfield; see FLAG constants. Additionally, if a command was
     * found, the two outparams are set.
     */
    static uint16_t readPacket(command_t *&command_dat_out,
                               command_t &command_len_out);

    /**
     * @brief Fills in the ID and timestamp given a subpacket pointer.
     * Note that this method expects that the buffer has already been
     * appropreiately sized.
     * To avoid needing to deal with move semantics, this method expects a
     * raw pointer rather than a unique pointer, so you'll need to call the
     * get() method of SubPktPtr.
     */
    static void addIdTime(PacketBuffWriter &buf, Ids id, uint32_t time) {
        buf.write(static_cast<uint8_t>(id));
        buf.write(&time, sizeof(time));
    }

    static void send();

    // A bit of magic SFINAE from stack overflow, for forwardCommand
    // https://stackoverflow.com/a/16824239/10762409

    // Primary template, used for meaningful error message
    // Falls back to this if operator() is not present
    template <typename T, typename...> struct can_receive_command {
      public:
        // type dependent false
        static_assert(!(std::is_same<T, T>::value),
                      "The passed in value for forwardCommand() must have the "
                      "method run_command ");

        static constexpr bool value = true;
    };

    // specialization that does the checking
    template <typename T> struct can_receive_command<T> {
      private:
        template <typename C>
        static constexpr auto check(C *) -> typename std::is_same<
            decltype(std::declval<C>().run_command(std::declval<uint8_t>())),
            void>::type;

        template <typename> static constexpr std::false_type check(...);

        typedef decltype(check<T>(nullptr)) is_ok;

      public:
        static constexpr bool value = is_ok::value;
    };
};
