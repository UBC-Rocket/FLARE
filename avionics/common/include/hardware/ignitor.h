#ifndef IGNITOR_H
#define IGNITOR_H

/**
 * Ignitor Class
 */

/*Includes------------------------------------------------------------*/
#include "hw-interface.h"
#include <cstdint>

/*Constants------------------------------------------------------------*/
#define IGNITOR_DELAY 25          // in milliseconds
#define CONTINUITY_CHECK_DELAY 50 // in microseconds
// tested at 11.1V with a 2.2k/470 divider (606) or 1.3k/330 (642)
//  or 1.33k/330 (629)
// threshold for a discontinuous ematch -> read voltage / 3.3 * 1023
#define DISCONTINUOUS_THRESHOLD 650

class Ignitor : public IParachute {
  public:
    /**
     * @brief ignitor constructor
     * @param ignitePin the pin for the ignitor
     * @param continuityPin the pin for checking continuity
     * @param continuityADCPin the pin for the ADC for checking continuity
     */
    Ignitor(uint8_t ignitePin, uint8_t continuityPin, uint8_t continuityADCPin)
        : ignitePin_(ignitePin), continuityPin_(continuityPin),
          continuityADCPin_(continuityADCPin) {
        /*init ignitor*/
        Hal::pinMode(ignitePin_, Hal::PinMode::OUTPUT);
        Hal::digitalWrite(ignitePin_, Hal::PinDigital::LOW);

        /*continuity check */
        Hal::pinMode(continuityPin_, Hal::PinMode::OUTPUT);

        Hal::digitalWrite(continuityPin_, Hal::PinDigital::HIGH);
        Hal::sleep_us(CONTINUITY_CHECK_DELAY);

        int continuity = Hal::analogRead(continuityADCPin);
        Hal::digitalWrite(continuityPin_, Hal::PinDigital::LOW);

        if (continuity <= DISCONTINUOUS_THRESHOLD) {
            status = HardwareStatus::FAILURE;
        } else {
            status = HardwareStatus::NOMINAL;
        }
    }

    // void init();
    void activate();
    bool isWorking();

  private:
    uint8_t ignitePin_;
    uint8_t continuityPin_;
    uint8_t continuityADCPin_;
};

#endif
