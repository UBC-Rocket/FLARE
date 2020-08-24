#pragma once

#include <atomic>
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
    // Everything is static but it's not a namespace b/c of the private member
    // variables
  private:
    typedef char Id;

    // static std::unordered_map<Id, std::mutex> m_mutexes;
    static std::mutex istream_mutex_; // lock before accessing istreams_
    static std::unordered_map<Id, std::queue<uint8_t>>
        istreams_; // Note - for input only

    // I got tired of thinking about the best way to pre-compose strings so
    // instead I'm just going to lock std::cout
    static std::mutex cout_mutex_;

    static std::thread input_;          // run infinite inputLoop()
    static std::atomic_bool run_input_; // controls if input_ is running

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
        const std::lock_guard<std::mutex> lock(istream_mutex_);

        if (istreams_[id].size() == 0) {
            return false;
        }
        c = istreams_[id].front();
        istreams_[id].pop();
        return true;
    }

    static int available(uint8_t id) {
        const std::lock_guard<std::mutex> lock(istream_mutex_);
        return istreams_[id].size();
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

    /**
     * @brief Corresponds to Request Analog Read packet in Confluence spec.
     * @param pin_id ID of the pin being read.
     * @return Read value.
     */
    static int requestAnalogRead(uint8_t const pin_id) {
        run_input_ = false; // input_ will now run at most once more
        putPacket('a', &pin_id,
                  1);  // ensures that at least one more SIM packet exists
        input_.join(); // so this won't block

        while (true) {
            // At this point program is single threaded, no need to acquire lock
            if (istreams_['a'].size() == 0) { // haven't heard back
                extractPacket();
            }
        }
        int val = istreams_['a'].front();
        istreams_['a'].pop();
        val *= 256;
        val += istreams_['a'].front();
        istreams_['a'].pop();

        // restart input loop
        std::thread input{&StdIoController::inputLoop};
        input_ = std::move(input);
        return val;
    }

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
        char c;
        while (true) {
            c = std::cin.get();
            if (std::cin.fail()) {
                std::cin.clear();
                continue;
            }

#ifdef OS_IS_WINDOWS
            // 0x0D is CR, 0x0A is LF
            if (c == 0x0D && std::cin.peek() == 0x0A) {
                std::cin.ignore(1);
                return 0x0A;
            }
#endif

            return c;
        }
    }

    static void extractPacket() {
        Id id;
        uint16_t length;
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
            const std::lock_guard<std::mutex> lock(istream_mutex_);
            for (auto j : buf) {
                istreams_[id].push(j);
            } // unlock mutex
        }
    }
    static void inputLoop() {
        while (run_input_) {
            extractPacket();
        }
    }
};
