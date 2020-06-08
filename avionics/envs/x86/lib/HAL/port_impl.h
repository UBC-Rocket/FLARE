#pragma once

#include <cstring>

#include "HAL/port.h"
#include "stdio_controller.hpp"

namespace Hal {

class Serial {
  private:
    typedef StdIoController StdIO;

  public:
    constexpr Serial(uint8_t const id) : M_IO_ID(id) {}

    void const begin(long baud) {}          // no-op for native
    bool const available() { return true; } // std::cout always available
    int const read() {
        return 0;
    } // TODO - make this read from multiplexed stdio
    void const write(const uint8_t *buffer, std::size_t size) {
        StdIO::putPacket(M_IO_ID, buffer, size);
    };
    // TODO - make these actually do something
    void const print(char *dat) {}
    void const println(char *dat) {}

    constexpr operator bool() { return true; };

  private:
    const uint8_t M_IO_ID;
};

static Serial SerialUSB(1);
static Serial SerialGPS(2);
static Serial SerialRadio(3);
static Serial IridiumSerial(4);
static Serial SerialCamera(5);
} // namespace Hal
