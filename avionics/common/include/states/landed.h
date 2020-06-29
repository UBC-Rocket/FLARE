#pragma once

/*Includes------------------------------------------------------------*/
#include "state_input_struct.h"
#include "state_interface.h"

namespace State {

class Landed : public IState {
  public:
    Landed() {}

    /*
     * @brief Return the assigned enumeration code.
     * @return Enumeration code.
     */
    StateId getStateEnum(void) { return StateId::LANDED; }

    /*
     * @brief Return the next state, based on input data (mostly from filtered
     * sensor data)
     * @return State enumeration code, to be passed into the std::map between
     * codes and used states. Note that the returned code may be the same state.
     */
    StateId getNewState(const StateInput &, StateAuxilliaryInfo &) {
        return StateId::LANDED;
    }
};

} // namespace State
