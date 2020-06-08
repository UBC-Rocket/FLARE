#pragma once

/*Includes------------------------------------------------------------*/
#include "state_input_struct.h"
#include "state_interface.h"

namespace State {

class Standby : public IState {
  public:
    /**
     * @brief Constructor
     * @param ascent_id State to go to if launch is detected in standby.
     * @param STANDBY_CHECKS Number of checks to make in a row before
     * determining that rocket has launched.
     * @param LAUNCH_THRESHOLD Altitude (AGL) above which launch detection
     * starts checking.
     */
    Standby(StateId const ascent_id, uint8_t const STANDBY_CHECKS,
            float const LAUNCH_THRESHOLD)
        : m_ascent_id(ascent_id), M_STANDBY_CHECKS(STANDBY_CHECKS),
          M_LAUNCH_THRESHOLD(LAUNCH_THRESHOLD) {}

    /**
     * @brief Return the assigned enumeration code.
     * @return Enumeration code.
     */
    StateId getStateEnum(void) { return StateId::STANDBY; }

    /**
     * @brief Return the next state, based on input data (mostly from filtered
     * sensor data)
     * @return State enumeration code, to be passed into the std::map between
     * codes and used states. Note that the returned code may be the same state.
     */
    StateId getNewState(StateInput const &input,
                        StateAuxilliaryInfo &state_aux);

  private:
    StateId m_ascent_id;
    int M_STANDBY_CHECKS;
    float M_LAUNCH_THRESHOLD;
};
} // namespace State
