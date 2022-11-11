#ifndef HARDWARE_IGNITOR_HPP_A22E53F7BA764FAC8FA3CEAB90131013
#define HARDWARE_IGNITOR_HPP_A22E53F7BA764FAC8FA3CEAB90131013

/**
 * Ignitor Class
 */

/*Includes------------------------------------------------------------*/
#include "gpio.h"
#include "hw-interface.h"
#include <cstdint>

/*Constants------------------------------------------------------------*/
constexpr short IGNITOR_DELAY = 25;          // in milliseconds
constexpr short CONTINUITY_CHECK_DELAY = 50; // in microseconds
// tested at 11.1V with a 2.2k/470 divider (606) or 1.3k/330 (642)
//  or 1.33k/330 (629)
// threshold for a discontinuous ematch -> read voltage / 3.3 * 1023
constexpr short DISCONTINUOUS_THRESHOLD = 650;

#undef INPUT
#undef OUTPUT
#undef LOW
#undef HIGH
#undef LED_BUILTIN

class Ignitor : public IIgnitor {
  public:
    /**
     * @brief ignitor Constructor. All used pins are initialized (mode is set)
     * here.
     * @param ignitePin the pin for the ignitor - when firing, this pin is
     * brought high
     * @param continuityPin the pin for checking continuity - when testing
     * continuity, this pin is brought high briefly
     * @param continuityADCPin the pin for the ADC for checking continuity -
     * when testing continuity, the value at this pin is read
     */
    Ignitor(Pin ignitePin, Pin continuityPin, Pin continuityADCPin);

    /**
     * @brief fires ignitor at pin ignitePin_
     */
    void fire() override;

  private:
    Pin ignitePin_;
    Pin continuityPin_;
    Pin continuityADCPin_;
};

#endif
