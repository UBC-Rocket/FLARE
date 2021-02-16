#pragma once

#include <unordered_map> //for std::unordered_map (hash map)

#include "calculations.h"
#include "state_interface.h"
#include "HAL/time.h"
#include "radio.h"

class StateMachine {
  public:
    typedef std::unordered_map<StateId, IState *> StateMap;

    StateMachine(StateMap map, StateId initial_state_)
        : current_id_(initial_state_), state_map_(map) {
        assert(map.count(StateId::STANDBY) != 0);
        assert(map.count(StateId::ARMED) != 0);
        assert(map.count(StateId::WINTER_CONTINGENCY) != 0);
    }

    void update(Calculator const &calc) {
        IState *current_state = state_map_[current_id_];
        StateId new_id = current_state->getNewState(calc);
        if (new_id != current_id_) {
            state_map_[new_id]->onEntry();
            Radio::sendState(Hal::tpoint_to_uint(Hal::now_ms()), static_cast<uint16_t>(new_id));
            current_id_ = new_id;
        }
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
