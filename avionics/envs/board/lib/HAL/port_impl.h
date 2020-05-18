#ifndef HAL__PORT_IMPL_H_4670E21514FE46DE8214D2F192FBBD20
#define HAL__PORT_IMPL_H_4670E21514FE46DE8214D2F192FBBD20

#include <HardwareSerial.h>

#include "HAL/port.h"

namespace Hal {

class Serial {
  public:
    constexpr Serial(HardwareSerial &seri) : m_seri(seri) {}

    void const begin(long baud) { m_seri.begin(baud); }
    bool const available() { return m_seri.available(); }
    int const read() { return m_seri.read(); }
    void const write(const uint8_t *buffer, size_t size) {
        m_seri.write(buffer, size);
    };
    void const print(char *dat) { m_seri.print(dat); }
    void const println(char *dat) { m_seri.println(dat); }

    HardwareSerial &getSerial() { return m_seri; }
    operator bool() { return bool(m_seri); };

  private:
    HardwareSerial m_seri;
};

// auto static SerialUSB       = Hal::Serial(Serial); //?
auto const static SerialGPS = Hal::Serial(Serial1);
auto static SerialRadio = Hal::Serial(Serial2);

auto static SerialCamera = Hal::Serial(Serial3);
auto const static IridiumSerial = Hal::Serial(Serial4);
// Annoyingly trying to make some of these SerialX const directly doesn't easily
// work because the underlying HardwareSerial isn't const, which I think means
// everything that uses HardwareSerial (i.e. everything useful) is not const,
// which kind of cascades upwards.

} // namespace Hal
#endif
