#pragma once

/*Includes------------------------------------------------------------*/
#include "state_input_struct.h"
#include "state_interface.h"

namespace State {

class PressureDelay : public IState {
  public:
    PressureDelay(uint32_t const DELAY_TIME_MS)
        : M_DELAY_TIME_MS(DELAY_TIME_MS) {}

    /*
     * @brief Return the assigned enumeration code.
     * @return Enumeration code.
     */
    StateId getStateEnum(void) { return StateId::PRESSURE_DELAY; }

    /*
     * @brief Return the next state, based on input data (mostly from filtered
     * sensor data)
     * @return State enumeration code, to be passed into the std::map between
     * codes and used states. Note that the returned code may be the same state.
     */
    StateId getNewState(const StateInput &input,
                        StateAuxilliaryInfo &state_aux);

  private:
    uint32_t const M_DELAY_TIME_MS;
};

} // namespace State
