/*Includes------------------------------------------------------------*/
#include <HAL/gpio.h>
#include <HAL/time.h>

#include "hardware/ignitor.h"


void Ignitor(uint8_t pin) {

}

void Ignitor::init() {
    /*init ignitor*/
    Hal::pinMode(ignitePin, Hal::PinMode::OUTPUT);
    Hal::digitalWrite(ignitePin, Hal::PinDigital::LOW);

    /*continuity check */
    Hal::pinMode(continuityPin, Hal::PinMode::OUTPUT);

    Hal::digitalWrite(continuityPin, Hal::PinDigital::HIGH);
    Hal::sleep_us(CONTINUITY_CHECK_DELAY);

    int continuity = Hal::analogRead(continuityADCPin);
    Hal::digitalWrite(continuityPin, Hal::PinDigital::LOW);

    if (continuity <= DISCONTINUOUS_THRESHOLD) {
        status = HardwareStatus::FAILURE;
    } else {
        status = HardwareStatus::NOMINAL;
    }
}

void Ignitor::activate() {
    Hal::digitalWrite(ignitePin, Hal::PinDigital::HIGH);
    Hal::sleep_ms(IGNITOR_DELAY);
    Hal::digitalWrite(ignitePin, Hal::PinDigital::LOW);
}
