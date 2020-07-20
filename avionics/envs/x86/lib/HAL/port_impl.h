#pragma once

#include <atomic>
#include <cstring>
#include <thread>

#include "HAL/port.h"
#include "stdio_controller.hpp"

namespace Hal {

class Serial {
  private:
    typedef StdIoController StdIO;

  public:
    Serial(uint8_t const id) : IO_ID_(id), buf_(), buffer_regular_thread_() {
        if (id != StdIoController::DEV_NULL) {
            buffer_regular_thread_ =
                std::move(std::thread(&Serial::buffer_send_loop, this));
        }
    }

    void begin(long) {} // no-op for native
    int available() const { return StdIO::available(IO_ID_); }
    int read();
    std::size_t write(const uint8_t *const inbuf, std::size_t const size);
    std::size_t write(uint8_t const b) { return write(&b, 1); }

    // TODO - make these actually do something
    // void const print(char *dat) {}
    // void const println(char *dat) {}

    constexpr operator bool() { return true; }

    // Useful for mocking
    Serial &getSerial() { return *this; }

  private:
    const uint8_t IO_ID_;
    constexpr static std::size_t BUFFER_SIZE = 1024;
    // constexpr static int SEND_PERIOD_MS = 100;

    std::array<uint8_t, BUFFER_SIZE> buf_;
    std::size_t buf_used_ = 0;
    bool buf_sent_ = false;
    std::mutex buf_mutex_; // All 3 of the above require this mutex
    std::thread buffer_regular_thread_;
    /**
     * \brief Loops and ensures that data is sent at least at 10 Hz.
     * To be run by another thread. Note that if data is already sent in a given
     * 100 ms interval, then it doesn't send.
     */
    void buffer_send_loop() {
        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            const std::lock_guard<std::mutex> lock(buf_mutex_);
            if (!buf_sent_ && buf_used_ != 0) {
                send_buffer();
            }
            buf_sent_ = false;
        }
    }

    /**
     * \brief Sends all data in the buffer. NOT THREAD SAFE
     * Before calling, acquire a std::lock_guard on `buf_mutex_`.
     */
    inline void send_buffer() {
        StdIO::putPacket(IO_ID_, buf_.begin(), buf_used_);
        buf_used_ = 0;
    }
};

class SerialInst {
    /**
     * @brief Collection of available serial objects, collected into a class to
     * hopefully prevent one instance from appearing for each time this file
     * gets #included
     */
  public:
    static Serial USB;
    static Serial GPS;
    static Serial Radio;
    static Serial Camera;
};
} // namespace Hal
