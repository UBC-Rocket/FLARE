#pragma once

#include "HAL/time.h"
#include <unordered_map>
#include "state_id_enum.hpp"
#include "event_id_enum.hpp"

namespace State {
    std::unordered_map<StateId, EventId> STATE_CHANGE_EVENTS {
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
