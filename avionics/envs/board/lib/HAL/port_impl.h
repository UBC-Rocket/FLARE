#pragma once

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

class SerialInst {
    /**
     * @brief Collection of available serial objects, collected into a class to
     * hopefully prevent one instance from appearing for each time this file
     * gets #included
     */
  public:
    // auto static SerialUSB//?
    static Hal::Serial GPS;
    static Hal::Serial Radio;
    static Hal::Serial Camera;
    // Annoyingly trying to make some of these SerialX const directly doesn't
    // easily work because the underlying HardwareSerial isn't const, which I
    // think means everything that uses HardwareSerial (i.e. everything useful)
    // is not const, which kind of cascades upwards.
};

} // namespace Hal
