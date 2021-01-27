#pragma once

#include <unordered_map> //for std::unordered_map (hash map)

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

    void update(const StateInput state_input, StateAuxilliaryInfo state_aux) {
        IState *current_state = state_map_[current_id_];
        StateId new_id = current_state->getNewState(state_input, state_aux);
        if (new_id != current_id_) {
            state_map_[new_id]->onEntry();
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
            state_map_[current_id_]->onEntry();
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
            state_map_[current_id_]->onEntry();
            return true;
        }
        return false;
    }

    /**
     * Set the state machine to a failsafe state
     */
    void abort() {
        current_id_ = StateId::WINTER_CONTINGENCY;
        state_map_[current_id_]->onEntry();
    }

    const StateId &getState() const { return current_id_; }

  private:
    StateId current_id_;
    StateMap state_map_;
};
