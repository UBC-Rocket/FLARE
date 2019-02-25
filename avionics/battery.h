/*Battery Sensor Header*/
#ifndef __battery_sensor__
#define __battery_sensor__

#include <Arduino.h>

#define MINIMUM_BATTERY_VOLTAGE 10
#define LOW_BATTERY_VOLTAGE 11

#define R1 44170  //Nominal: 44200. Higher is larger range.
#define R2 15150  //Nominal: 16000. Lower is larger range.
//Maximum voltage = 3.3*(R1+R2)/R2

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
