/*Includes------------------------------------------------------------*/
#include "log.hpp"
#include "radio.h"
#include <HAL/pin_util.h>
#include <HAL/time.h>

#include "hardware/ignitor.h"

Ignitor::Ignitor(Pin ignitePin, Pin continuityPin) : ignitePin_(ignitePin), continuityPin_(continuityPin) {
    /*init ignitor*/
    Hal::pinMode(ignitePin_, Hal::PinMode::OUTPUT);
    Hal::digitalWrite(ignitePin_, Hal::PinDigital::LO);

    /*continuity check */
    pinMode(static_cast<uint32_t>(continuityPin_), 0); // 0 = digitalRead mode from Arduino's wiring_digital.c file
    int continuity = digitalRead(static_cast<uint32_t>(continuityPin_));

    #ifdef TESTING
        FLOG_DEBUG("Continuity read for ignitor on pin: ", static_cast<uint8_t>(ignitePin_));
        FLOG_DEBUG("Continuity value: ", continuity);
    #endif

    if (continuity) {
        status = HardwareStatus::FAILURE;
    } else {
        status = HardwareStatus::NOMINAL;
    }
}

void Ignitor::fire() {
    Hal::digitalWrite(ignitePin_, Hal::PinDigital::HI);
    Hal::sleep_ms(IGNITOR_DELAY);
    Hal::digitalWrite(ignitePin_, Hal::PinDigital::LO);

    // TODO(akoen) This could be expanded to support different events for
    // different ignitions.
    Radio::sendEvent(Hal::tpoint_to_uint(Hal::now_ms()),
                     EventId::IGNITOR_FIRED);
    FLOG_INFO("Firing ignitor at pin: ", static_cast<int>(ignitePin_)); 
    FLOG_INFO("At time (ms): ", static_cast<std::int32_t>(Hal::millis()));
}
