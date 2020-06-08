#pragma once

/*Includes------------------------------------------------------------*/
#include "state_input_struct.h"
#include "state_interface.h"

namespace State {

class DrogueDescent : public IState {
  public:
    /*
     * @param MAIN_DEPLOY_ALTITUDE altitude to deploy main parachute at, in
     * metres
     */
    DrogueDescent(float const MAIN_DEPLOY_ALTITUDE, int8_t DEPLOY_CHECKS)
        : M_MAIN_DEPLOY_ALTITUDE(MAIN_DEPLOY_ALTITUDE),
          M_DEPLOY_CHECKS(DEPLOY_CHECKS) {}

    /*
     * @brief Return the assigned enumeration code.
     * @return Enumeration code.
     */
    StateId getStateEnum(void) { return StateId::DROGUE_DESCENT; }

    /*
     * @brief Return the next state, based on input data (mostly from filtered
     * sensor data)
     * @return State enumeration code, to be passed into the std::map between
     * codes and used states. Note that the returned code may be the same state.
     */
    StateId getNewState(const StateInput &input,
                        StateAuxilliaryInfo &state_aux);

  private:
    float const M_MAIN_DEPLOY_ALTITUDE;
    int8_t const M_DEPLOY_CHECKS;
};

} // namespace State
