#pragma once

#include <array>
#include <chrono>
#include <cstring>

#include "HAL/port.h"
#include "stdio_controller.hpp"

namespace Hal {

class SerialInst;

class Serial {
  private:
    friend SerialInst;

    typedef StdIoController StdIO;
    const uint8_t IO_ID_;
    constexpr static std::size_t BUFFER_SIZE = 1024;

    typedef std::array<uint8_t, BUFFER_SIZE> Buffer;
    typedef uint8_t SeriID;
    typedef std::chrono::steady_clock Clock;
    constexpr static std::chrono::milliseconds MAX_SEND_PERIOD_{100};
    // constexpr static std::chrono::milliseconds MAX_SEND_PERIOD_;
    Buffer buf_;
    std::size_t buf_used_;
    Clock::time_point last_sent_;

    static std::vector<Serial *> serials_;

  public:
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

    /**
     * \brief Goes over all Serial instances and sens if they haven't been sent
     * in the past MAX_SEND_PERIOD_.
     */
    static void watchdog_callback() {
        for (auto seri : serials_) {
            if (Clock::now() - seri->last_sent_ > MAX_SEND_PERIOD_) {
                seri->send_buffer();
            }
        }
    }

  private:
    Serial(uint8_t const id) : IO_ID_(id) {
        if (id != StdIoController::DEV_NULL) {
            serials_.push_back(this);
        }
    }

    /**
     * \brief Sends all data in the buffer. NOT THREAD SAFE
     * Before calling, acquire a std::lock_guard on `buf_mutex_`.
     */
    inline void send_buffer() {
        // StdIO::putPacket(IO_ID_, bufs_[IO_ID_].begin(), buf_useds_[IO_ID_]);
        StdIO::putPacket(IO_ID_, buf_.begin(), buf_used_);
        buf_used_ = 0;
        last_sent_ = Clock::now();
    }
};

class SerialInst {
    /**
     * @brief Collection of available serial objects, collected into a class
     * to hopefully prevent one instance from appearing for each time this
     * file gets #included
     */
  public:
    static Serial USB;
    static Serial GPS;
    static Serial Radio;
    static Serial Camera;
    // static Serial Serial;
};
} // namespace Hal
