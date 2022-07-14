#pragma once

#include <functional> //for std::hash specialization (need to specialize)

#include "calculations.h"
#include "state_id_enum.hpp"
#include "HAL/time.h"

// Need to specify hash for StateId, since scoped enums aren't allowed as hash
// keys until C++14
namespace std { // this is one of like three places where this is allowed
template <> struct hash<StateId> {
    std::size_t operator()(const StateId &t) const {
        return std::hash<int>{}(static_cast<int>(t));
    }
};
} // namespace std

class IState {
  public:
    /**
     * @brief Return the assigned enumeration code.
     * @return Enumeration code.
     */
    virtual StateId getStateEnum(void) = 0;

    /**
     * @brief Return the next state, based on input data (mostly from filtered
     * sensor data)
     * @return State enumeration code, to be passed into the std::map between
     * codes and used states. Note that the returned code may be the same state.
     */
    virtual StateId getNewState(Calculator const &input, Hal::t_point const launch_time) = 0;

    /**
     * @brief Optional method that runs when entering a state from a different
     * state.
     */
    virtual void onEntry(){};
};
