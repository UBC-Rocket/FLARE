/*Includes------------------------------------------------------------*/
#include "log.hpp"
#include "radio.h"
#include <HAL/pin_util.h>
#include <HAL/time.h>

#include "hardware/ignitor.h"

Ignitor::Ignitor(Pin ignitePin, Pin continuityPin, Pin continuityADCPin)
    : ignitePin_(ignitePin), continuityPin_(continuityPin),
      continuityADCPin_(continuityADCPin) {
    /*init ignitor*/
    Hal::pinMode(ignitePin_, Hal::PinMode::OUTPUT);
    Hal::digitalWrite(ignitePin_, Hal::PinDigital::LO);

    /*continuity check */
    Hal::pinMode(continuityADCPin_, Hal::PinMode::INPUT);
    int continuity = Hal::analogRead(continuityADCPin_);

    #ifdef TESTING
        Serial.print("Continuity read for ignitor on pin "); 
        Serial.print(static_cast<uint8_t>(ignitePin_));
        Serial.print(": "); 
        Serial.println(continuity);
    #endif
    

    if (continuity <= DISCONTINUOUS_THRESHOLD) {
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
    LOG_INFO("Firing ignitor at pin "
             << static_cast<int>(ignitePin_) << " at time "
             << static_cast<std::int32_t>(Hal::millis()) << "ms");
}
