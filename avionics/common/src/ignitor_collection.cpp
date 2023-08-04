#include "ignitor_collection.h"
#include "gpio.h"
#include "log.hpp"
#include "options.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#endif


IgnitorCollection::IgnitorCollection()
    : drogue(Pin::DROGUE_IGNITOR, Pin::DROGUE_CONTINUITY_TEST,
             Pin::DROGUE_CONTINUITY_READ),
      main(Pin::MAIN_IGNITOR, Pin::MAIN_CONTINUITY_TEST,
           Pin::MAIN_CONTINUITY_READ) {
    status_ = RocketStatus::NOMINAL;

    // TODO: Check continuity continuously instead of only during startup
    if (main.getStatus() == ComponentStatus::FAILURE) {
        #ifdef TESTING
        Serial.println("Main parachute ignitor failed");
        #endif

        LOG_ERROR("Broken ignitor for main parachute");
        status_bitfield_[0] |= 0x80;
        // status_ = RocketStatus::CRITICAL_FAILURE;
    }
    if (drogue.getStatus() == ComponentStatus::FAILURE) {
        #ifdef TESTING
        Serial.println("Drogue parachute ignitor failed");
        #endif

        LOG_ERROR("Broken ignitor for drogue parachute");
        status_bitfield_[0] |= 0x40;
        // status_ = RocketStatus::CRITICAL_FAILURE;
    }
}
