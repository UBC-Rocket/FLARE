#pragma once

/*Includes------------------------------------------------------------*/
#include "state_interface.h"

namespace State {

class WinterContingency : public IState {
  public:

  /**
   * @brief Construct a new state object
   */
    WinterContingency() {}

    /**
     * @brief Return the assigned enumeration code.
     * @return Enumeration code.
     */
    StateId getStateEnum(void) { return StateId::WINTER_CONTINGENCY; }

    /**
     * @brief Return the next state, based on input data (mostly from filtered
     * sensor data)
     * @return State enumeration code, to be passed into the std::map between
     * codes and used states. Note that the returned code may be the same state.
     */
    StateId getNewState(Calculator const &) {
        return StateId::WINTER_CONTINGENCY;
    }
};

} // namespace State
