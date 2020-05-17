#ifndef HAL__PORT_IMPL_H_4670E21514FE46DE8214D2F192FBBD20
#define HAL__PORT_IMPL_H_4670E21514FE46DE8214D2F192FBBD20

#include "HAL/port.h"
#include "stdio_controller.hpp"
#include <cstring>

namespace global {
StdIoController extern stdio_controller;
}

namespace Hal {

class Serial : public ISerial {
  public:
    Serial(uint8_t id) : m_io{::global::stdio_controller}, M_IO_ID(id) {}

    void begin(long baud) {}          // no-op for native
    bool available() { return true; } // std::cout always available
    int read() { return 0; } // TODO - make this read from multiplexed stdio
    void write(const uint8_t *buffer, std::size_t size) {
        m_io.putPacket(M_IO_ID, buffer, size);
    };
    // TODO - make these actually do something
    void print(char *dat) {}
    void println(char *dat) {}

    operator bool() { return true; };

  private:
    uint8_t M_IO_ID;
    StdIoController const &m_io; // alias
};

} // namespace Hal
#endif
