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

} // namespace Hal
#endif
