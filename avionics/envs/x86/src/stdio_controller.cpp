#include <cassert>
#include <fstream>

#include "stdio_controller.hpp"

std::thread StdIoController::m_input{};
std::unordered_map<StdIoController::Id, std::mutex>
    StdIoController::m_mutexes{};
std::unordered_map<StdIoController::Id, std::queue<uint8_t>>
    StdIoController::m_istreams{};
std::mutex StdIoController::m_cout{};
std::ofstream StdIoController::out_log_{
    "FW_SIM_log", std::ios_base::out | std::ios_base::binary};

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

void StdIoController::putPacket(uint8_t const id, char const *c,
                                uint16_t const length) {
    const std::lock_guard<std::mutex> lock(m_cout);
    // TODO - check the success of std::cout.put and other unformatted
    // output, and possibly do something about it

    // Windows converts lf -> crlf. To get around this, we force every
    // character to be put one at a time -- this means a stream like CRLF
    // becomes CRLFLF. Then on the reciving end replace CRLF with LF to
    // recover the original values.
    output(id);                             // ID
    output(static_cast<char>(length >> 8)); // Length, bigendian
    output(static_cast<char>(length & 0xFF));

    // Data
    for (char const *end = c + length; c != end; c++)
        output(*c);

    std::cout << std::flush;
    out_log_.flush();
}
void StdIoController::putPacket(uint8_t const id, uint8_t const *c,
                                uint16_t const length) {
    putPacket(id, reinterpret_cast<const char *>(c), length);
    // Shoudn't need to worry that std::cin et al thinks its pointing to a
    // char - the bits should come out as defined by uint8_t, which would be
    // expected. Char is defined to be a byte, so this is fine as long as a
    // byte is 8 bits (no seriously there are some machines that have bytes
    // with more than 8 bits.)
    // TODO - maybe less sloppily deal with this problem without using
    // reinterpret_cast? Although we're low enough that reinterpret is close
    // to being allowed
}

void StdIoController::output(char const c) {
    std::cout.put(c);
    out_log_.put(c);
}
