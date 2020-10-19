#pragma once

#include <atomic>
#include <bits/stdint-uintn.h>
#include <cstring> //for memmove
#include <fstream>
#include <iostream> //for std::cin/cout
#include <mutex>
#include <queue>         // for queue
#include <thread>        // for non-blocking input
#include <type_traits>   // for is_same
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

    enum class PacketIds : uint8_t {
        analog_read = 0x61,
        sensor_read = 0x71,
    };

    static constexpr auto FLOAT_SIZE = sizeof(float);
    static_assert(FLOAT_SIZE == 4,
                  "Code assumes floating point numbers are 4 bytes");

    // at least until someone can make sure that this is the right way to do
    // this.
    static_assert(std::is_same<uint8_t, unsigned char>::value, "Code assumes uint8_t can be used for bytes");

    class BlockingRequest {
        /**
         * @brief Helper class - constructor sends a packet request on
         *stream_id, and blocks until a response is heard. Destructor restarts
         *the input, so as long as your instance of this object is in scope, you
         *have clean, single-threaded access to the incoming data.
         **/
      public:
        BlockingRequest(const uint8_t stream_id, const uint8_t *const packet,
                        const std::size_t packet_len) {
            // input_ will now run at most once more
            StdIoController::run_input_ = false;
            // ensures that at least one more SIM packet exists
            StdIoController::putPacket(stream_id, packet, packet_len);
            input_.join(); // so this won't block

            // Keep polling for new packet until response is received.
            while (istreams_[stream_id].size() == 0) {
                extractPacket();
            }
        }

        // Can't copy.
        BlockingRequest(const BlockingRequest &) = delete;
        BlockingRequest &operator=(const BlockingRequest &other) = delete;

        ~BlockingRequest() {
            StdIoController::run_input_ = true;
            std::thread input{&StdIoController::inputLoop};
            StdIoController::input_ = std::move(input);
        }
    };

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
        uint8_t packet_id = static_cast<uint8_t>(PacketIds::analog_read);
        BlockingRequest restart(packet_id, &pin_id, 1);

        int val = istreams_[packet_id].front();
        istreams_[packet_id].pop();
        val *= 256;
        val += istreams_[packet_id].front();
        istreams_[packet_id].pop();

        return val;
    }

    /**
     * @brief Corresponds to Request Sensor Read packet in Confluence spec.
     * @param sensor_id ID of the desired sensor.
     * @param num_floats the number of floats to read.
     * @return array of sensor measurements.
     */
    static std::vector<float> requestSensorRead(uint8_t sensor_id,
                                                std::size_t num_floats) {
        uint8_t packet_id = static_cast<uint8_t>(PacketIds::sensor_read);
        BlockingRequest restart(packet_id, &sensor_id, 1);

        std::vector<float> sensor_data;
        for (std::size_t f = 0; f < num_floats; f++) {
            uint8_t packetData[FLOAT_SIZE];
            for (std::size_t i = 0; i < FLOAT_SIZE; i++) {
                packetData[i] = istreams_[packet_id].front();
                istreams_[packet_id].pop();
            }
            sensor_data.push_back(charsToFloat(packetData));
        }

        return sensor_data;
    }

  private:
    /**
     * @brief reinterprets a big endian array of 4 chars to a floating point
     * number.
     * @param data an array of 4 chars with the most significant byte first.
     * @return the floating point representation
     */
    static float charsToFloat(uint8_t data[4]) {
        float f;
        std::memcpy(&f, &data, FLOAT_SIZE);
        return f;
    }

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
