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
#include "event_id_enum.hpp"
#include "ignitor_collection.h"
#include "sensor_collection.h"
#include "sensors/GPS.h"
#include "sensors/IMU.h"
#include "sensors/accelerometer.h"
#include "state_id_enum.hpp"
#include "subpktptr.h"

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
    typedef uint16_t fwd_cmd_t;
    enum class Ids {
        status_ping = 0x00,
        message = 0x01,
        event = 0x02,
        config = 0x03,
        gps = 0x04,
        state = 0x05,
        bulk_sensor = 0x30,
    };

    constexpr static Hal::ms WATCHDOG_SEND_INTERVAL{3000};
    constexpr static fwd_cmd_t STOP_PARSE_FLAG = 1 << 0;
    constexpr static fwd_cmd_t CAN_SEND_FLAG = 1 << 1;

    static bool can_send_; // Is XBee available to send

  public:
    // type RadioMembers is public for technical reasons but cannot be used
    // (especially since no definition is given)
    class RadioMembers;

    // Loose type to more explicitly indicate what variables are commands.
    typedef uint8_t command_t;

    /**
     * @brief Initializes the class.
     * @param SERIAL_RADIO Uninitialized HardwareSerial used for radio
     (e.g.
     * SerialRadio)
     * @param MAX_QUEUED_BYTES Maximum number of subpacket data bytes to
     queue
     * before dropping the oldest subpackets.
     * @param MAX_PACKETS_PER_RX_LOOP Maximum number of packets to send
     every
     * time forwardCommand() is called.
     */
    static void initialize();

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

        read_count_ = 0; // resetting
        fwd_cmd_t result;

        while (true) {
            command_t *command_data;
            uint8_t command_len;
            result = readPacket(command_data, command_len);
            can_send_ |= result & CAN_SEND_FLAG;
            if (result & STOP_PARSE_FLAG) {
                break;
            }
            for (int i = 0; i < command_len; ++i) {
                command_receiver.run_command(command_data[i]);
            }
        }

        if (Hal::now_ms() - watchdog_last_send > WATCHDOG_SEND_INTERVAL) {
            can_send_ = true;
        }

        if (can_send_) {
            can_send_ = false;
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
                               IMU &imu, GPS &gps, uint16_t state_id);

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
    static void sendState(const uint32_t time, uint16_t state_id);

    static void sendConfig(const uint32_t time);

    /**
     * @brief Helper function to send an event. See radio specification.
     * @param time Timestamp, in milliseconds
     * @param event, the event id to send
     */
    static void sendEvent(const uint32_t time, const EventId event);
    // TODO: Implement stage separation packet when separation implemented.

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
    static fwd_cmd_t readPacket(command_t *&command_dat_out,
                                uint8_t &command_len_out);

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

    /**
     * @brief Actually writes out data to the radio serial line; this is done
     * infrequently to maximize usage of radio packets.
     */
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
