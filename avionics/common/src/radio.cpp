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

#include "radio.h"

#include "HAL/port_impl.h"

#include "XBee.h"
#include "ignitor_collection.h"
#include "radio_queue.h"
#include "sensor_collection.h"

// Some defines that are actually done in CMake, but set here to get
// Intellisense to stop yelling.

#ifndef RADIO_CONFIG_PACKET_SIM_ACTIVE
#define RADIO_CONFIG_PACKET_SIM_ACTIVE 2
#endif

#ifndef RADIO_CONFIG_PACKET_ROCKET_ID
#define RADIO_CONFIG_PACKET_ROCKET_ID 255
#endif

#ifndef RADIO_CONFIG_PACKET_VERSION_STR
#define RADIO_CONFIG_PACKET_VERSION_STR                                        \
    "Fake version string; quiet Intellisense "
#endif

namespace {
constexpr uint64_t GND_STN_ADDR_MSB = 0x0013A200;
constexpr uint64_t GND_STN_ADDR_LSB = 0x41678FC0;
constexpr uint32_t RADIO_BAUD_RATE = 921600;
constexpr uint8_t MAX_PACKETS_PER_RX_LOOP = 8;
constexpr int MAX_QUEUED_BYTES = 800;

} // namespace

/**
 * The purpose of RadioMembers is to have a set of static member variables for
 * Radio, that are not visible from the interface (.h file), and also
 * maintaining a mechanism similar to constructors, in that there is a defined
 * set of operations that occurs automatically. Specifically, the instance
 * `self` is this set of member variables.
 *
 * The challenge is that some private methods of Radio are used; as such
 * RadioMembers must be a friend of Radio. In the interest of not polluting the
 * global namespace, the type is nested. To further enforce access control, only
 * Radio is permitted to access private members, which are the only useful
 * component.
 *
 * In order to actually make RadioMembers class accessible from the outside,
 * it's listed as a public type under the Radio class definition, but no
 * definition is given for RadioMembers so it's not actually usable (outside of
 * this translation unit). Technically you might be able to provide a little
 * more protection by e.g. marking the type as protected then extracting it by
 * inheriting from Radio, and there might be other tricks you could pull, but I
 * couldn't think of any / was too lazy to try them.
 */
class Radio::RadioMembers {
    friend class Radio;

  public:
    RadioMembers();

  private:
    XBee xbee_;
    XBeeAddress64 gnd_addr_;
    ZBTxRequest tx_packet_;
    ZBRxResponse rx;

    std::unordered_map<StateId, EventId> state_change_events;

    RadioQueue tx_q_; // the [ueue] is silent :)
    uint8_t payload_[RADIO_MAX_SUBPACKET_SIZE];
};
static Radio::RadioMembers self;

constexpr Hal::ms Radio::WATCHDOG_SEND_INTERVAL;

// Initializes radio member variables;
Radio::RadioMembers::RadioMembers()
    : gnd_addr_(GND_STN_ADDR_MSB, GND_STN_ADDR_LSB),
      tx_q_(MAX_QUEUED_BYTES),
      state_change_events ({
          {StateId::POWERED_ASCENT, EventId::LAUNCH},
          {StateId::MACH_LOCK, EventId::MACH_LOCK_ENTER},
          {StateId::ASCENT_TO_APOGEE, EventId::MACH_LOCK_EXIT},
          {StateId::PRESSURE_DELAY, EventId::APOGEE},
          {StateId::DROGUE_DESCENT, EventId::DROGUE_DEPLOY},
          {StateId::MAIN_DESCENT, EventId::MAIN_DEPLOY},
          {StateId::LANDED, EventId::LAND}
}) {}

void Radio::initialize() {
    auto &serial = Hal::SerialInst::Radio;
    serial.begin(RADIO_BAUD_RATE);
    while (!serial)
        ;
    self.xbee_.setSerial(serial.getSerial());
    self.tx_packet_.setAddress64(self.gnd_addr_);
    self.tx_packet_.setPayload(self.payload_);
    Radio::sendMessage(Hal::millis(), "Radio initialized");
    Radio::send();
}

void Radio::send() {
    if (!self.tx_q_.empty()) {
        self.tx_packet_.setPayloadLength(self.tx_q_.fillPayload(self.payload_));
        self.xbee_.send(self.tx_packet_);
    }
}

void Radio::sendStatus(uint32_t time, RocketStatus status,
                       SensorCollection &sensors, IgnitorCollection &ignitors) {
    PacketBuffWriter buf;
    addIdTime(buf, Ids::status_ping, time);

    buf.write(&status, 1);

    buf.write(sensors.getStatusBitfield(), 1);
    buf.write(sensors.getStatusBitfield() + 1, 1);
    buf.write(ignitors.getStatusBitfield(), 1);
    buf.write(ignitors.getStatusBitfield() + 1, 1);

    Radio::addSubpacket(std::move(buf.packet));
}

void Radio::sendBulkSensor(uint32_t time, float alt, Accelerometer &xl,
                           IMU &imu, GPS &gps, uint8_t state_id) {
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

    Radio::addSubpacket(std::move(buf.packet));
}

void Radio::sendMessage(const uint32_t time, const char *str) {
    PacketBuffWriter buf;
    addIdTime(buf, Ids::message, time);

    uint8_t strlen = std::strlen(str);
    buf.write(strlen);
    buf.write(str, strlen);

    Radio::addSubpacket(std::move(buf.packet));
}

void Radio::sendGPS(const uint32_t time, GPS &gps) {
    PacketBuffWriter buf;
    addIdTime(buf, Ids::gps, time);

    buf.write(gps.getData(), 12);

    Radio::addSubpacket(std::move(buf.packet));
}

void Radio::sendSingleSensor(const uint32_t time, uint8_t id, float data) {
    PacketBuffWriter buf;
    buf.write(id);
    buf.write(&time, sizeof(time));

    buf.write(&data, 4);

    Radio::addSubpacket(std::move(buf.packet));
}

void Radio::sendState(const uint32_t time, uint8_t state_id) {
    PacketBuffWriter buf;

    buf.write(0x1D); // id
    buf.write(&time, 4);
    buf.write(0);
    buf.write(0);
    buf.write(0);
    buf.write(state_id);

    Radio::addSubpacket(std::move(buf.packet));
}

void Radio::sendConfig(const uint32_t time) {
    PacketBuffWriter buf;
    addIdTime(buf, Ids::config, time);

    // Defined in CMakeLists/platformio.ini
    buf.write(RADIO_CONFIG_PACKET_SIM_ACTIVE);
    buf.write(RADIO_CONFIG_PACKET_ROCKET_ID);

    // -1 because null terminated string
    constexpr size_t len = sizeof(RADIO_CONFIG_PACKET_VERSION_STR) - 1;
    static_assert(len == 40, "RADIO_CONFIG_PACKET_VERSION_STR incorrect size!");
    uint8_t version_bytes[40] = {0};
    std::memcpy(version_bytes, RADIO_CONFIG_PACKET_VERSION_STR, len);

    buf.write(version_bytes, sizeof(version_bytes));

    Radio::addSubpacket(std::move(buf.packet));
}

void Radio::sendEvent(const uint32_t time, const EventId event) {
    PacketBuffWriter buf;
    addIdTime(buf, Ids::event, time);
    buf.write(&event, sizeof(uint16_t));
    Radio::addSubpacket(std::move(buf.packet));
}

void Radio::sendEventStateChange(const uint32_t time, const StateId state) {
    EventId event = self.state_change_events.find(state)->second;
    sendEvent(time, event);
}

void Radio::addSubpacket(SubPktPtr dat) { self.tx_q_.push(std::move(dat)); }

int Radio::read_count_ = 0;

fwd_cmd_t Radio::readPacket(command_t *&command_dat_out,
                            command_t &command_len_out) {
    fwd_cmd_t result = 0;
    while (read_count_ < MAX_PACKETS_PER_RX_LOOP) {
        self.xbee_.readPacket();
        read_count_++;
        if (!(self.xbee_.getResponse().isAvailable() ||
              self.xbee_.getResponse().isError())) {
            break;
        }

        if (self.xbee_.getResponse().isError()) {
            // TODO - figure out whether there's anything
            // we should do about Xbee errors / log error
        } else if (self.xbee_.getResponse().getApiId() ==
                   ZB_TX_STATUS_RESPONSE) {
            result |= CAN_SEND_FLAG;
            // If we get 2 responses in a row, implies previously we sent an
            // extra one, so we shouldn't respond twice again.
        } else if (self.xbee_.getResponse().getApiId() == ZB_RX_RESPONSE) {
            // received command from xbee_
            self.xbee_.getResponse().getZBRxResponse(self.rx);
            command_dat_out = self.rx.getData();
            command_len_out = self.rx.getDataLength();
            return result;
        } else {
            // TODO - log unrecognized API Id
        }
    }
    result |= STOP_PARSE_FLAG;
    return result;
}
