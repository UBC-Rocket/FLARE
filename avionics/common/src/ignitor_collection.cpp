#include "ignitor_collection.h"
#include "gpio.h"
#include "log.hpp"

IgnitorCollection::IgnitorCollection()
    : drogue(
        Pin::DROGUE_IGNITOR, 
        Pin::DROGUE_CONTINUITY_TEST, 
        Pin::DROGUE_CONTINUITY_READ,
        false
    ),
      redundantDrogue(
        Pin::DROGUE_REDUNDANT_IGNITOR, 
        Pin::DROGUE_REDUNDANT_CONTINUITY_TEST, 
        Pin::DROGUE_REDUNDANT_CONTINUITY_READ,
        true
    ),
      main(
        Pin::MAIN_IGNITOR, 
        Pin::MAIN_CONTINUITY_TEST,
        Pin::MAIN_CONTINUITY_READ,
        false
    ), redundantMain(
        Pin::MAIN_REDUNDANT_IGNITOR,
        Pin::MAIN_REDUNDANT_CONTINUITY_TEST,
        Pin::MAIN_REDUNDANT_CONTINUITY_READ,
        true
    ) {
    status_ = RocketStatus::NOMINAL;

    if (main.getStatus() == ComponentStatus::FAILURE) {
        LOG_ERROR("Broken ignitor for main parachute");
        status_bitfield_[0] |= 0x80;
        status_ = RocketStatus::CRITICAL_FAILURE;
    }
    if (redundantMain.getStatus() == ComponentStatus::FAILURE) {
        LOG_ERROR("Broken redundant ignitor for main parachute");
        status_bitfield_[0] |= 0x80;
        status_ = RocketStatus::CRITICAL_FAILURE;
    }
    if (drogue.getStatus() == ComponentStatus::FAILURE) {
        LOG_ERROR("Broken ignitor for drogue parachute");
        status_bitfield_[0] |= 0x40;
        status_ = RocketStatus::CRITICAL_FAILURE;
    }
    if (redundantDrogue.getStatus() == ComponentStatus::FAILURE) {
        LOG_ERROR("Broken redundant ignitor for drogue parachute");
        status_bitfield_[0] |= 0x40;
        status_ = RocketStatus::CRITICAL_FAILURE;
    }
}
