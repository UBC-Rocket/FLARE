#include <cassert>

#include "stdio_controller.hpp"

std::thread StdIoController::m_input{};
std::unordered_map<StdIoController::Id, std::mutex>
    StdIoController::m_mutexes{};
std::unordered_map<StdIoController::Id, std::queue<uint8_t>>
    StdIoController::m_istreams{};
std::mutex StdIoController::m_cout{};

void StdIoController::initialize() {
    static bool done = false;
    if (done) { // idempotency
        return;
    }
    done = true;
    std::cout << "SYN" << std::flush;
    char ack[4] = "ACK";
    for (auto i = 0; i < 3; i++) {
        assert(ack[i] == std::cin.get());
    }
    putConfigPacket();
    std::thread input{&StdIoController::inputLoop};
    m_input = std::move(input);
}
