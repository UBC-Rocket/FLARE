#ifndef HAL__PORT_IMPL_H_4670E21514FE46DE8214D2F192FBBD20
#define HAL__PORT_IMPL_H_4670E21514FE46DE8214D2F192FBBD20

#include <HardwareSerial.h>

#include "Hal/port.h"

namespace Hal {

class Serial : public ISerial {
public:
    Serial(HardwareSerial &seri) : m_seri(seri) {}

    void begin(long baud) { m_seri.begin(baud); }
    bool available() { return m_seri.available(); }
    int read() { return m_seri.read(); }
    void write(const uint8_t *buffer, size_t size) {
        m_seri.write(buffer, size);
    };
    void print(char *dat) { m_seri.print(dat); }
    void println(char *dat) { m_seri.println(dat); }

    HardwareSerial &getSerial() {return m_seri;}
    operator bool() { return bool(m_seri); };

private:
    HardwareSerial m_seri;

};

}
#endif
