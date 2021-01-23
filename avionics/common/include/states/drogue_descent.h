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
    DrogueDescent(StateId post_descent, float const MAIN_DEPLOY_ALTITUDE,
                  int8_t DEPLOY_CHECKS)
        : MAIN_DEPLOY_ALTITUDE_(MAIN_DEPLOY_ALTITUDE),
          DEPLOY_CHECKS_(DEPLOY_CHECKS), post_descent_(post_descent) {}

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
    float const MAIN_DEPLOY_ALTITUDE_;
    int8_t const DEPLOY_CHECKS_;
    const StateId post_descent_;
};

} // namespace State
