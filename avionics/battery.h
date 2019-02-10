#ifndef __battery_sensor__
#define __battery_sensor__

#include <Arduino.h>

#define MINIMUM_BATTERY_VOLTAGE 10
#define LOW_BATTERY_VOLTAGE 11

#define R1 32700
#define R2 14100

class Battery
{
    public:  // public functions

    // Constructor. batterySensorPin is the pin for battery sensor (i.e. output of the voltage divider)
    Battery(byte batterySensorPin)
    {
        m_divider = static_cast<float>(R2) /(R1 + R2);
        m_batterySensorPin = batterySensorPin;
    }

    float getVoltage();

    private:

    float m_divider;
    byte m_batterySensorPin;
};

#endif
