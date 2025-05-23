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
#include <cassert>
#include <cstring>
#include <utility> //for std::move
#include <SPI.h>

#include "HAL/port_impl.h"

// #include "XBee.h"
#include "LoRa.h"
#include "log.hpp"
#include "ignitor_collection.h"
#include "radio.h"
#include "roar/buffer.hpp"
#include "sensor_collection.h"
#include "state_id_enum.hpp"

// Some defines that are actually done in CMake, but set here to get
// Intellisense to stop yelling.

// #ifndef RADIO_CONFIG_PACKET_SIM_ACTIVE
// #define RADIO_CONFIG_PACKET_SIM_ACTIVE 2
// #endif

// #ifndef RADIO_CONFIG_PACKET_ROCKET_ID
// #define RADIO_CONFIG_PACKET_ROCKET_ID 255
// #endif

// #ifndef RADIO_CONFIG_PACKET_VERSION_STR
// #define RADIO_CONFIG_PACKET_VERSION_STR                                        \
//     "Fake version string; quiet Intellisense "
// #endif

// TODO PLACE THIS SOMEWHERE ELSE
#define LORA_CS_PIN     204  // PC4
#define LORA_RESET_PIN  195  // PB0
#define LORA_IRQ_PIN    203  // PB1
#define LORA_MOSI_PIN   198  // PA7
#define LORA_MISO_PIN   199  // PA6
#define LORA_SCK_PIN    205  // PA5

namespace {
    constexpr int txPower = 20;
    constexpr int spreadingFactor = 12;
    constexpr long signalBandwidth = 125E3;
    constexpr int codingRateDenominator = 8;
    constexpr long preambleLength = 8;
    constexpr uint8_t gain = 6;
    constexpr long loraFrequency = 900E6;
    constexpr uint32_t loraInitError = 50;
    constexpr int maxQueuedBytes = 800;
    constexpr int maxQueuedSubpkts = maxQueuedBytes / 20; // 20 bytes per subpkt on average
    constexpr int packetPayloadSpace = 255;

    // *** constexpr uint32_t kDefaultFlaregunAddrMsb = 0x0013A200;
    // *** constexpr uint32_t kDefaultFlaregunAddrLsb = 0x41678FC0;
    // *** constexpr uint32_t kRadioBaudRate = 921600;
    // *** constexpr uint8_t kMaxPacketsPerRxLoop = 8;
    // *** constexpr int kMaxQueuedBytes = 800;
    // *** // 20 bytes per subpkt on average
    // *** constexpr int kMaxQueuedSubpkts = kMaxQueuedBytes / 20;

} // namespace
// *** constexpr unsigned short kPacketPayloadSpace = 255;

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
    RadioMembers()
        : spi_(LORA_MOSI_PIN, LORA_MISO_PIN, LORA_SCK_PIN),
          tx_q_(packetPayloadSpace, maxQueuedBytes, maxQueuedSubpkts) {}
        // : tx_q_(kPacketPayloadSpace, kMaxQueuedBytes, kMaxQueuedSubpkts),
//           tx_packet_(
//               XBeeAddress64(kDefaultFlaregunAddrMsb, kDefaultFlaregunAddrLsb),
//               payload_, kPacketPayloadSpace) {}

  private:
    SPIClass spi_;
    roar::Buffer tx_q_; // the [ueue] is silent :)
    uint8_t payload_[packetPayloadSpace];

//     XBee xbee_;
//     ZBTxRequest tx_packet_;
//     ZBRxResponse rx;

// ***  public:
// ***    RadioMembers()
// ***        : tx_q_(kPacketPayloadSpace, kMaxQueuedBytes, kMaxQueuedSubpkts),
// ***          tx_packet_(
// ***              XBeeAddress64(kDefaultFlaregunAddrMsb, kDefaultFlaregunAddrLsb),
// ***              payload_, kPacketPayloadSpace) {}
// ***
// ***  private:
// ***    roar::Buffer tx_q_; // the [ueue] is silent :)
// ***    uint8_t payload_[kPacketPayloadSpace];
// ***
// ***    XBee xbee_;
// ***    ZBTxRequest tx_packet_;
// ***    ZBRxResponse rx;
};
static Radio::RadioMembers self;

constexpr Hal::ms Radio::WATCHDOG_SEND_INTERVAL;
bool Radio::can_send_ = true;

void Radio::initialize() {
    // Initialize the LoRa radio
    LoRa.setTxPower(txPower);
    LoRa.setSpreadingFactor(spreadingFactor);
    LoRa.setSignalBandwidth(signalBandwidth);
    LoRa.setCodingRate4(codingRateDenominator);
    LoRa.setPreambleLength(preambleLength);
    LoRa.setGain(gain);

    // Set the pins for the LoRa module
    LoRa.setSPI(self.spi_);
    LoRa.setPins(LORA_CS_PIN, LORA_RESET_PIN, LORA_IRQ_PIN);

    // Initialize the LoRa module
    if (!LoRa.begin(loraFrequency)) {
        #ifdef TESTING
            LOG_WARN("Failed to initalize LoRa!");
        #endif
        while (!LoRa.begin(loraFrequency)) {
            #ifdef TESTING
                LOG_ERROR("FAILED TO INIT RADIO!");
            #endif
            Hal::sleep_ms(loraInitError);
        }
    } else {
        #ifdef TESTING
            LOG_INFO("LoRa initalized!");
        #endif
    }

    Radio::sendMessage(Hal::millis(), "Radio initialized");

    // *** auto &serial = Hal::SerialInst::Radio;
    // *** serial.begin(kRadioBaudRate);
    // *** while (!serial)
    // ***     ;

    // *** self.xbee_.setSerial(serial);

    // *** Radio::sendMessage(Hal::millis(), "Radio initialized");
    // *** Radio::send();
}

void Radio::addIdTime(command_t id, uint32_t time) {
    // self.tx_q_.write(static_cast<uint8_t>(id));
    unsigned long totalSeconds = time / 1000;
    uint8_t seconds = totalSeconds % 60;
    uint8_t minutes = (totalSeconds / 60) % 60;
    uint8_t hours = totalSeconds / 3600;
    self.tx_q_.write(&hours, 1);
    self.tx_q_.write(&minutes, 1);
    self.tx_q_.write(&seconds, 1);
}

void Radio::send() {
    if (!self.tx_q_.empty()) {
        LoRa.beginPacket();
        int payload_len = self.tx_q_.fillPayload(self.payload_);
        LoRa.beginPacket();
        LoRa.write(self.payload_, payload_len);
        LoRa.endPacket();
    }

    // *** if (!self.tx_q_.empty()) {
    // ***     self.tx_packet_.setPayloadLength(self.tx_q_.fillPayload(self.payload_));
    // ***     self.xbee_.send(self.tx_packet_);
    // *** }
}

void Radio::sendStatus(uint32_t time, RocketStatus status,
                       SensorCollection &sensors, IgnitorCollection &ignitors) {
    self.tx_q_.allocSubpkt(10);
    addIdTime(command_t::status_ping, time);

    self.tx_q_.write(static_cast<uint8_t>(status));
    self.tx_q_.write(sensors.getStatusBitfield(), 2);
    self.tx_q_.write(ignitors.getStatusBitfield(), 2);

    send();
}

void Radio::sendBulkSensor(uint32_t time, float alt, Accelerometer &xl,
                           IMU &imu, GPS &gps, uint16_t state_id) {
    self.tx_q_.allocSubpkt(9);
    addIdTime(command_t::bulk_sensor, time);

    // Altitude
    self.tx_q_.write(&alt, 4);

    // Accelerometer
    // self.tx_q_.write(xl.getData(), 12);

    // // IMU // TODO - check that these are the correct 3 floats to send for
    // // orientation
    // self.tx_q_.write(imu.getData(), 12);

    // GPS
    // self.tx_q_.write(gps.getData(), 8);

    // State
    self.tx_q_.write(&state_id, sizeof(uint16_t));

    send();
}

void Radio::sendMessage(const uint32_t time, const char *str) {
    auto strlen = std::strlen(str);
    assert(strlen + 5 <= kPacketPayloadSpace);

    self.tx_q_.allocSubpkt(6 + strlen);
    addIdTime(command_t::message, time);

    self.tx_q_.write(strlen);
    self.tx_q_.write(str, strlen);

    send();
}

void Radio::sendGPS(const uint32_t time, GPS &gps) {
    self.tx_q_.allocSubpkt(17);

    addIdTime(command_t::gps, time);
    self.tx_q_.write(gps.getData(), 12);

    send();
}

void Radio::sendSingleSensor(const uint32_t time, uint8_t id, float data) {
    self.tx_q_.allocSubpkt(9);

    self.tx_q_.write(id);
    self.tx_q_.write(&time, sizeof(time));
    self.tx_q_.write(&data, 4);

    send();
}

void Radio::sendState(const uint32_t time, uint16_t state_id) {
    self.tx_q_.allocSubpkt(7);

    addIdTime(command_t::state, time);
    self.tx_q_.write(&state_id, sizeof(state_id));

    send();
}

void Radio::sendConfig(const uint32_t time) {
    self.tx_q_.allocSubpkt(47);
    addIdTime(command_t::config, time);

    // Defined in CMakeLists/platformio.ini
    self.tx_q_.write(RADIO_CONFIG_PACKET_SIM_ACTIVE);
    self.tx_q_.write(RADIO_CONFIG_PACKET_ROCKET_ID);

    // -1 because null terminated string
    constexpr size_t len = sizeof(RADIO_CONFIG_PACKET_VERSION_STR) - 1;
    static_assert(len == 40, "RADIO_CONFIG_PACKET_VERSION_STR incorrect size!");

    self.tx_q_.write(RADIO_CONFIG_PACKET_VERSION_STR, len);

    send();
}

void Radio::sendEvent(const uint32_t time, const EventId event) {
    self.tx_q_.allocSubpkt(7);

    addIdTime(command_t::event, time);
    self.tx_q_.write(&event, sizeof(uint16_t));

    send();
}

int Radio::read_count_ = 0;

Radio::fwd_cmd_t Radio::readPacket(command_t *&command_dat_out,
                                   uint8_t &command_len_out) {

    // TODO - implement this
    return 0;

    // *** fwd_cmd_t result = 0;
    // *** command_len_out = 0;

    // *** while (read_count_ < kMaxPacketsPerRxLoop) {
    // ***     self.xbee_.readPacket();
    // ***     read_count_++;
    // ***     if (!(self.xbee_.getResponse().isAvailable() ||
    // ***           self.xbee_.getResponse().isError())) {
    // ***         break;
    // ***     }

    // ***     if (self.xbee_.getResponse().isError()) {
    // ***         // TODO - figure out whether there's anything
    // ***         // we should do about Xbee errors / log error
    // ***     } else if (self.xbee_.getResponse().getApiId() ==
    // ***                ZB_TX_STATUS_RESPONSE) {
    // ***         result |= CAN_SEND_FLAG;
    // ***         // If we get 2 responses in a row, implies previously we sent an
    // ***         // extra one, so we shouldn't respond twice again.
    // ***     } else if (self.xbee_.getResponse().getApiId() == ZB_RX_RESPONSE) {
    // ***         // received command from xbee_
    // ***         self.xbee_.getResponse().getZBRxResponse(self.rx);
    // ***         command_dat_out = (command_t *) self.rx.getData();
    // ***         command_len_out = self.rx.getDataLength();
    // ***         return result;
    // ***     } else {
    // ***         // TODO - log unrecognized API Id
    // ***     }
    // *** }
    // *** result |= STOP_PARSE_FLAG;
    // *** return result;
}

void Radio::updateAddress() {
    // TODO - implement this

    // *** self.tx_packet_.setAddress64(self.rx.getRemoteAddress64());
}
