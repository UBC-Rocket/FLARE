#pragma once

#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

#include "radio_interface.h"
#include "radio_queue.h"
#include "stdio_controller.hpp"

class RadioController : public RadioControllerBase {
  private:
    typedef StdIoController StdIO;

  public:
    RadioController(Hal::Serial &serial_radio,
                    unsigned short const MAX_QUEUED_BYTES = 800,
                    uint8_t const MAX_PACKETS_PER_RX_LOOP = 8)
        : RadioControllerBase(MAX_QUEUED_BYTES) {}

    void listenAndAct() {
        uint8_t command;

        while (StdIO::get(M_IO_ID, command)) {
            // TODO - do something with command
        }

        if (m_tx_q.empty()) {
            // Nothing to send - we're done
            return;
        }

        uint8_t len = m_tx_q.fillPayload(m_payload);
        StdIO::putPacket(M_IO_ID, m_payload, len);
    }

  private:
    std::queue<uint8_t> m_commands;
    uint8_t m_payload[256];

    // TODO use serial_radio, not this
    static uint8_t const M_IO_ID = 0x52;
};
