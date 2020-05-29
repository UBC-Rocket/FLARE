/*Includes------------------------------------------------------------*/
#include <HAL/gpio.h>
#include <HAL/time.h>

#include "hardware/ignitor.h"

// void Ignitor(uint8_t pin) {}

// void Ignitor::init() {}

bool Ignitor::isWorking() {
    if (status == HardwareStatus::FAILURE) {
        return false;
    } else {
        return true;
    }
}

void Ignitor::activate() {
    Hal::digitalWrite(ignitePin, Hal::PinDigital::HIGH);
    Hal::sleep_ms(IGNITOR_DELAY);
    Hal::digitalWrite(ignitePin, Hal::PinDigital::LOW);
}
