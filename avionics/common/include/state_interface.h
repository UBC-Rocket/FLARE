#pragma once

#include "state_input_struct.h"
#include <functional> //for std::hash specialization (need to specialize)

enum class StateId {
    STANDBY,
    ARMED,
    POWERED_ASCENT,
    PRE_AIR_START_COAST_TIMED,
    ASCENT_TO_APOGEE,
    MACH_LOCK,
    PRESSURE_DELAY,
    DROGUE_DESCENT,
    MAIN_DESCENT,
    LANDED,
    WINTER_CONTINGENCY
};

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
    /*
     * @brief Return the assigned enumeration code.
     * @return Enumeration code.
     */
    virtual StateId getStateEnum(void) = 0;

    /*
     * @brief Return the next state, based on input data (mostly from filtered
     * sensor data)
     * @return State enumeration code, to be passed into the std::map between
     * codes and used states. Note that the returned code may be the same state.
     */
    virtual StateId getNewState(const StateInput &input,
                                StateAuxilliaryInfo &state_aux) = 0;
};
