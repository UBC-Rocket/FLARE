/*Battery Sensor Header*/
#ifndef __battery_sensor__
#define __battery_sensor__

#include <Arduino.h>
#include "sensors.h"

class Battery
{
    public:  // public functions
        // Constructor. batterySensorPin is the pin for battery sensor (i.e. output of the voltage divider)
        Battery(byte batterySensorPin);
        float getVoltage();
        OverallError getStatus();

    private:
        float m_divider;
        byte m_batterySensorPin;
};

#endif
