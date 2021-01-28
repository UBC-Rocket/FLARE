#pragma once

#include "HAL/time.h"
#include <map>
#include "state_id_enum.hpp"
#include "event_id_enum.hpp"

namespace State {

    // REVIEW: An unorderd_map should work here, but causes compilation errors
    // for an unknown reason.
    const std::map<StateId, EventId> UPDATE_EVENTS {
        {StateId::STANDBY, EventId::STANDBY},
        {StateId::ARMED, EventId::ARMED},
        {StateId::POWERED_ASCENT, EventId::POWERED_ASCENT},
        {StateId::MACH_LOCK, EventId::MACH_LOCK},
        {StateId::ASCENT_TO_APOGEE, EventId::ASCENT_TO_APOGEE},
        {StateId::PRESSURE_DELAY, EventId::PRESSURE_DELAY},
        {StateId::DROGUE_DESCENT, EventId::DROGUE_DESCENT},
        {StateId::MAIN_DESCENT, EventId::MAIN_DESCENT},
        {StateId::LANDED, EventId::LANDED},
        {StateId::WINTER_CONTINGENCY, EventId::WINTER_CONTINGENCY}
    };
}
