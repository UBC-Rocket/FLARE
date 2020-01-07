/*Includes------------------------------------------------------------*/
#include <Utility/gpio.h>
#include <Utility/time.h>

#include "hardware/ignitor.h"


void Ignitor(uint8_t pin) {

}

void Ignitor::init() {
    /*init ignitor*/
    Utility::pinMode(ignitePin, Utility::PinMode::OUTPUT);
    Utility::digitalWrite(ignitePin, Utility::PinDigital::LOW);

    /*continuity check */
    Utility::pinMode(continuityPin, Utility::PinMode::OUTPUT);

    Utility::digitalWrite(continuityPin, Utility::PinDigital::HIGH);
    Utility::sleep_us(CONTINUITY_CHECK_DELAY);

    int continuity = Utility::analogRead(continuityADCPin);
    Utility::digitalWrite(continuityPin, Utility::PinDigital::LOW);

    if (continuity <= DISCONTINUOUS_THRESHOLD) {
        status = HardwareStatus::FAILURE;
    } else {
        status = HardwareStatus::NOMINAL;
    }
}

void Ignitor::activate() {
    Utility::digitalWrite(ignitePin, Utility::PinDigital::HIGH);
    Utility::sleep_ms(IGNITOR_DELAY);
    Utility::digitalWrite(ignitePin, Utility::PinDigital::LOW);
}
