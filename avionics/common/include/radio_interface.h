#pragma once

#include <assert.h>
#include <cstring>

#include "radio_queue.h"
#include "sensor_collection.h"
#include "sensors/GPS.h"
#include "sensors/IMU.h"
#include "sensors/accelerometer.h"
#include "status.h"

class RadioControllerBase {
  private:
    enum class Ids {
        status_ping = 0x00,
        message = 0x01,
        bulk_sensor = 0x30,
    };

  public:
    RadioControllerBase(unsigned short const MAX_QUEUED_BYTES = 800)
        : m_tx_q(MAX_QUEUED_BYTES) {}
    /**
     * Add a subpacket to the queue to be sent. Note that this is a rather
     * low-level utility; there should also be a helper method for any given
     * subpacket that will build up the specific format this needs that you
     * should use instead.
     * @param dat A SubPktPtr (refer to typedef) containing the data.
     */
    void addSubpacket(SubPktPtr dat) { m_tx_q.push(std::move(dat)); }

    /**
     * @brief Meat of the action - listens for any incoming packets, then
     * transmits data and performs rocket actions as necessary.
     */
    virtual void listenAndAct() = 0;

    /**
     * @brief Helper function to send status.
     */
    void sendStatus(uint32_t time, Status status, SensorCollection &sensors) {
        SubPktPtr buf(new std::vector<uint8_t>);
        buf->resize(8);

        setupIdTime(buf.get(), Ids::status_ping, time);

        *(buf->data() + 5) |= (static_cast<uint8_t>(status) << 6);

        // uint8_t byte_pos = 5;

        // Status message spec limits size of sensors to 22 (3 bytes * 8 bits -
        // 2 bits reserved for overall status).
        assert(sensors.NUM_SENSORS <= 22);
        uint8_t *sensor_bits = sensors.getStatusBitfield();

        // There's probably a proper way to do this with loops n stuff but
        // I'm to lazy to figure it out
        *(buf->data() + 5) |= (sensor_bits[0] >> 2);
        *(buf->data() + 6) |= (sensor_bits[0] << 6);
        *(buf->data() + 6) |= (sensor_bits[1] >> 2);
        *(buf->data() + 7) |= (sensor_bits[1] << 6);
        *(buf->data() + 7) |= (sensor_bits[2] >> 2);

        // TODO Include status of E-match components
    }

    /**
     * @brief Helper function to send bulk sensor data.
     */
    void sendBulkSensor(uint32_t time, float alt, Accelerometer &xl, IMU imu,
                        GPS gps, uint8_t state_id) {
        SubPktPtr buf(new std::vector<uint8_t>);
        buf->resize(42);

        setupIdTime(buf.get(), Ids::bulk_sensor, time);

        // Altitude
        std::memcpy(buf->data() + 5, &alt, 4);

        // Accelerometer
        std::memcpy(buf->data() + 9, xl.getData(), 12);

        // IMU // TODO - check that these are the correct 3 floats to send for
        // orientation
        std::memcpy(buf->data() + 21, imu.getData(), 12);

        // GPS
        std::memcpy(buf->data() + 33, gps.getData(), 8);

        // State
        std::memcpy(buf->data() + 41, &state_id, 1);

        addSubpacket(std::move(buf));
    }

  protected:
    /**
     * @brief Fills in the ID and timestamp given a subpacket pointer.
     * Note that this method expects that the buffer has already been
     * appropreiately sized.
     * To avoid needing to deal with move semantics, this method expects a raw
     * pointer rather than a unique pointer, so you'll need to call the get()
     * method of SubPktPtr.
     */
    void setupIdTime(std::vector<uint8_t> *buf, Ids id, uint32_t time) {
        (*buf)[0] = static_cast<uint8_t>(id);
        std::memcpy(buf->data() + 1, &time, 4);
    }
    RadioQueue m_tx_q;
};
