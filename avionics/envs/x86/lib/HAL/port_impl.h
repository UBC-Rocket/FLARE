#ifndef HAL__PORT_IMPL_H_4670E21514FE46DE8214D2F192FBBD20
#define HAL__PORT_IMPL_H_4670E21514FE46DE8214D2F192FBBD20

#include "Hal/port.h"

namespace Hal {

class NativeSerial : public ISerial {
public:
    Serial() : m_seri(seri) {}

    void begin(long baud) { } //no-op for native
    bool available() { return true; } //std::cout always available
    int read() { return m_seri.read(); }
    void write(const uint8_t *buffer, size_t size) {
        m_seri.write(buffer, size);
    };
    void print(char *dat) { m_seri.print(dat); }
    void println(char *dat) { m_seri.println(dat); }

    operator bool() { return bool(m_seri); };

private:

}

}
#endif
