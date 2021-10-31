#pragma once

#include <cassert>
#include <unordered_map> //for std::unordered_map (hash map)
#include <utility>

#include "HAL/time.h"
#include "calculations.h"
#include "log.hpp"
#include "radio.h"
#include "state_id_enum.hpp"
#include "state_interface.h"

class StateMachine {
  public:
    typedef std::unordered_map<StateId, IState *> StateMap;

    StateMachine(StateMap map, StateId initial_state_)
        : current_id_(initial_state_), state_map_(map) {
        assert(map.count(StateId::STANDBY) != 0);
        assert(map.count(StateId::ARMED) != 0);
        assert(map.count(StateId::WINTER_CONTINGENCY) != 0);
    }

    /**
     * @brief Updates the state machine, using new physical state information
     * provided by `Calculator`.
     * @param calc Calculator object, which should be freshly updated with new
     * sensor information
     * @return std::pair of old and new StateId (which may be identical). To
     * detect a state change, check whether the two returned values are
     * different.
     */
    std::pair<StateId, StateId> update(Calculator const &calc) {
        const StateId old_id = current_id_;
        IState *current_state = state_map_[old_id];
        const StateId new_id = current_state->getNewState(calc);
        if (new_id != old_id) {
            LOG_INFO("State changed (previous state "
                     << static_cast<std::int32_t>(old_id) << ", new state "
                     << static_cast<std::int32_t>(new_id) << ", time is "
                     << static_cast<std::int32_t>(Hal::millis()) << "ms)");
            state_map_[new_id]->onEntry();
            Radio::sendState(Hal::tpoint_to_uint(Hal::now_ms()),
                             static_cast<uint16_t>(new_id));
            current_id_ = new_id;
        }
        return std::make_pair(old_id, new_id);
    }

    /**
     * Arm the state machine. Only allowed if in STANDBY.
     * Returns true if arming was successful, false otherwise (i.e. not in
     * STANDBY)
     */
    bool arm() {
        if (current_id_ == StateId::STANDBY) {
            current_id_ = StateId::ARMED;
            return true;
        }
        return false;
    }

    /**
     * Disarm the state machine. Only allowed if in ARMED.
     * Returns true if disarming was successful, false otherwise (i.e. not in
     * ARMED)
     */
    bool disarm() {
        if (current_id_ == StateId::ARMED) {
            current_id_ = StateId::STANDBY;
            return true;
        }
        return false;
    }

    /**
     * Set the state machine to a failsafe state
     */
    void abort() { current_id_ = StateId::WINTER_CONTINGENCY; }

    const StateId &getState() const { return current_id_; }

  private:
    StateId current_id_;
    StateMap state_map_;
};
