#ifndef RADIO_INTERFACE_H_D7F3F9B440C74CEAB19571C6D763314B
#define RADIO_INTERFACE_H_D7F3F9B440C74CEAB19571C6D763314B

#include <cstring>
#include "radio_queue.h"
#include "sensors/accelerometer.h"
#include "sensors/GPS.h"
#include "sensors/IMU.h"

class IRadioController {
public:
    IRadioController(unsigned short const MAX_QUEUED_BYTES = 800) : m_tx_q(MAX_QUEUED_BYTES){}

    /**
     * Add a subpacket to the queue to be sent. Note that this is a rather low-level utility; there should also be a helper method for any given subpacket that will build up the specific format this needs that you should use instead.
     * @param dat A SubPktPtr (refer to typedef) containing the data.
     */
    void addSubpacket(SubPktPtr dat){
        m_tx_q.push(std::move(dat));
    }

    /**
     * @brief Meat of the action - listens for any incoming packets, then transmits data and performs rocket actions as necessary.
     */
    virtual void listenAndAct() = 0;

    /**
     * @brief Helper function to send bulk sensor data.
     */
    void sendBulkSensor(uint32_t time, float alt, Accelerometer &xl, IMU imu,GPS gps, uint8_t state_id) {
        SubPktPtr buf(new std::vector<uint8_t>);
        buf->resize(42);


        (*buf)[0] = 0x30; //ID

        // Time
        std::memcpy(buf->data() + 1, &time, 4);

        // Altitude
        std::memcpy(buf->data() + 5, &alt, 4);

        // Accelerometer
        std::memcpy(buf->data() + 9, xl.getData(), 12);

        // IMU //TODO - check that these are the correct 3 floats to send for orientation
        std::memcpy(buf->data() + 21, imu.getData(), 12);

        // GPS
        std::memcpy(buf->data() + 33, gps.getData(), 8);

        // State
        std::memcpy(buf->data() + 41, &state_id, 1);

        addSubpacket(std::move(buf));
    }


protected:
    RadioQueue m_tx_q;
};

#endif
