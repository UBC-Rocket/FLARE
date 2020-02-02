#ifndef RADIO_H_22181C75119344C48BDF0D5A550D2A02
#define RADIO_H_22181C75119344C48BDF0D5A550D2A02

#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include "radio_interface.h"
#include "stdio_controller.hpp"

class NativeRadioController : public IRadioController {
public:
    NativeRadioController(unsigned short const MAX_QUEUED_BYTES, StdIoController &io_controller) :
        m_io_controller(io_controller),
        IRadioController(MAX_QUEUED_BYTES) {}

    void listenAndAct(){
        uint8_t command;
        while(m_io_controller.get(M_IO_ID, command)){
            //TODO - do something with command
        }

        if m_tx_q.empty(){
            //Nothing to send - we're done
            return;
        }

        uint8_t len = m_tx_q.fillPayload(m_payload);
        m_io_controller.putPacket(M_IO_ID, m_payload, len);
    }

private:
    std::queue<uint8_t> m_commands;
    uint8_t m_payload[256];
    StdIoController &m_io_controller;

    static uint8_t const M_IO_ID = 0x52;
};


#endif
