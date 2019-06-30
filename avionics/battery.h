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

        /**
         * @brief  Gets the battery voltage level. Maximum voltage readable is 11.0 volts; to adjust, change the resistors used in the voltage divider.
         * @param  None
         * @return float batteryVoltage - voltage of the battery, in volts.
         */
        float getVoltage();

        /**
         * @brief  Gets the error status that the battery should report
         * @param  None
         * @return OverallError - battery status
         */
        OverallError getStatus();

    private:
        float m_divider;
        byte m_batterySensorPin;
};

#endif
