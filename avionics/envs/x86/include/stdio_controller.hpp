#pragma once

#include <cstring> //for memmove
#include <fstream>
#include <iostream> //for std::cin/cout
#include <mutex>
#include <queue>         // for queue
#include <thread>        // for non-blocking input
#include <unordered_map> // for hash map
#include <vector>

#if defined(WIN32) || defined(_WIN32)
#define OS_IS_WINDOWS
#endif

class StdIoController {
  private:
    typedef char Id;

    static std::unordered_map<Id, std::mutex> m_mutexes;
    static std::unordered_map<Id, std::queue<uint8_t>>
        m_istreams; // Note - for input only

    // I got tired of thinking about the best way to pre-compose strings so
    // instead I'm just going to lock std::cout
    static std::mutex m_cout;

    static std::thread m_input; // run infinite inputLoop()
    // Everything is static but it's not a namespace b/c of the private member
    // variables

    static std::ofstream out_log_; // logs happen before CRCRLF
    static void output(char const c);

  public:
    /**
     * @brief sets up SIM
     * Things that are setup: config packet is sent, with 2 way handshake.
     */
    static void initialize();

    constexpr static uint8_t DEV_NULL = 0;
    /**
     * @brief Attempts to extract a single character from the specified buffer
     * Id.
     * @param id ID being used (see documentation on stdio multiplexing spec:
     * http://confluence.ubcrocket.com/display/AV/Specs)
     * @param c If read succeeded, the result is written to c.
     * @return True if read succeeded, false if EOF.
     */
    static bool get(uint8_t id, uint8_t &c) {
        const std::lock_guard<std::mutex> lock(m_mutexes[id]);

        if (m_istreams[id].size() == 0) {
            return false;
        }
        c = m_istreams[id].front();
        m_istreams[id].pop();
        return true;
    }

    static int available(uint8_t id) {
        const std::lock_guard<std::mutex> lock(m_mutexes[id]);
        return m_istreams[id].size();
    }

    /**
     * @brief Basic level utility to put a packet into std::cout.
     * @param id ID being used
     * @param c Pointer to array of data to be sent
     * @param length Length of the data to be sent
     */
    static void putPacket(uint8_t const id, char const *c,
                          uint16_t const length);

    /**
     * @brief Basic level utility to put a packet into std::cout.
     * @param id ID being used
     * @param c Pointer to array of data to be sent
     * @param length Length of the data to be sent
     */
    static void putPacket(uint8_t const id, uint8_t const *c,
                          uint16_t const length);

  private:
    /**
     * @brief Helper function for configuration packet.
     */
    static void putConfigPacket() {
        uint8_t id = 0x01;
        uint32_t int_test = 0x04030201;
        float float_test = -2.0; // 0xC000 0000;

        char buf[8];
        std::memmove(buf, &int_test, 4);
        std::memmove(buf + 4, &float_test, 4);
        putPacket(id, buf, 8);
    }

    static char getCinForce() {
        char c[2];
        std::cin.getline(c, 2, '\0');
// getline always appends '\0' to the end, so if the character actually is '\0'
// then it will be discarded, but '\0' will still be returned
#ifdef OS_IS_WINDOWS
        // 0x0D is CR, 0x0A is LF
        if (c[0] == 0x0D && std::cin.peek() == 0x0A) {
            std::cin.ignore(1);
            return 0x0A;
        }
#endif

        return c[0];
    }

    static void inputLoop() {
        Id id;
        uint16_t length;

        for (;;) {
            id = getCinForce();
            length = getCinForce();
            length <<= 8;
            length |= getCinForce();

            auto buf = std::vector<char>();
            buf.reserve(length);
            for (int i = 0; i < length; i++) {
                buf.push_back(getCinForce());
            }

            { // scope for lock-guard
                const std::lock_guard<std::mutex> lock(m_mutexes[id]);
                for (auto j : buf) {
                    m_istreams[id].push(j);
                } // unlock mutex
            }
        }
    }
};
