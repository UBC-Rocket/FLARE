#pragma once

#include "HAL/time.h"
#include "radio.h"
#include "state_interface.h"

namespace State {
    const std::unordered_map<StateId, EventId> STATE_CHANGE_EVENTS = {
        {StateId::ARMED, EventId::ARMED},
        {StateId::STANDBY, EventId::DISARMED},
        {StateId::POWERED_ASCENT, EventId::LAUNCH},
        {StateId::MACH_LOCK, EventId::MACH_LOCK_ENTER},
        {StateId::ASCENT_TO_APOGEE, EventId::MACH_LOCK_EXIT},
        {StateId::PRESSURE_DELAY, EventId::APOGEE},
        {StateId::DROGUE_DESCENT, EventId::DROGUE_DEPLOY},
        {StateId::MAIN_DESCENT, EventId::MAIN_DEPLOY},
        {StateId::LANDED, EventId::LANDED},
        {StateId::WINTER_CONTINGENCY, EventId::ABORTED}
    };

    /**
     * @brief Sends the event packet associated with the transition to new_state
     * @param new_state the new state
     */
    inline void sendStateChangeEvent(StateId new_state) {
        Radio::sendEvent(Hal::tpoint_to_uint(Hal::now_ms()), STATE_CHANGE_EVENTS.find(new_state)->second);
    }
}
