#include "ignitor_collection.h"
#include "gpio.h"
// Ignitor(Pin ignitePin, Pin continuityPin, Pin continuityADCPin);
IgnitorCollection::IgnitorCollection()
    : main(Pin::MAIN_IGNITOR, Pin::MAIN_CONTINUITY_TEST,
           Pin::MAIN_CONTINUITY_READ),
      drogue(Pin::DROGUE_IGNITOR, Pin::DROGUE_CONTINUITY_TEST,
             Pin::DROGUE_CONTINUITY_READ) {
    status_ = RocketStatus::NOMINAL;
    if (main.getStatus() == ComponentStatus::FAILURE) {
        status_bitfield_[0] |= 0x80;
        status_ = RocketStatus::CRITICAL_FAILURE;
    }
    if (drogue.getStatus() == ComponentStatus::FAILURE) {
        status_bitfield_[0] |= 0x40;
        status_ = RocketStatus::CRITICAL_FAILURE;
    }
}

IgnitorCollectionPtr getIgnitors() {
    std::unique_ptr<IgnitorCollection> tmp(new IgnitorCollection);
    return std::move(tmp);
}
