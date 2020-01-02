#ifndef RADIO_H_22181C75119344C48BDF0D5A550D2A02
#define RADIO_H_22181C75119344C48BDF0D5A550D2A02

#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include "radio_interface.h"

class NativeRadioController : public IRadioController {
public:
    NativeRadioController(unsigned short const MAX_QUEUED_BYTES) :
        m_input(&NativeRadioController::inputToCommandQ, this), IRadioController(MAX_QUEUED_BYTES) {}

    void listenAndAct(){
        m_tx_q.fillPayload(m_payload);
        std::cout << m_payload;
        m_commands_mutex.lock();
        while(!m_commands.empty()){
            // m_commands.front(); //do something with command
            m_commands.pop();
        }
        m_commands_mutex.unlock();
    }

private:
    void inputToCommandQ(){
        char c;
        while(true){
            if (std::cin.get(c)) {
                m_commands_mutex.lock();
				m_commands.push(reinterpret_cast<uint8_t>(c));
                m_commands_mutex.unlock();
			}
        }
    }

    std::queue m_commands;
    uint8_t m_payload[256];
    std::thread m_input;
    std::mutex m_commands_mutex;
}


#endif
