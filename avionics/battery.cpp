#include "battery.h"
#include "Arduino.h"

/**
  * @brief  Gets the battery voltage level. Maximum voltage readable is 11.0 volts; to adjust, change the resistors used in the voltage divider.
  * @param  None
  * @return float batteryVoltage - voltage of the battery, in volts.
  */
float Battery::getVoltage()
{
    float inputValue = analogRead(m_batterySensorPin);
    // map it to the range the analog out:
    float teensyVoltage = map(inputValue, 0, 1023, 0, 3300); // maps input voltage 0-1023 to 0-3300 scale. Max 3300mV = 3.3V is the highest voltage Teensy can read.
    float batteryVoltage = (teensyVoltage / m_divider) / 1000; // converts output value from mV to V and divides by voltage divider value to calculate battery input voltage.
    return batteryVoltage;
}

