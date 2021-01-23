#pragma once

/*Includes------------------------------------------------------------*/
#include "state_input_struct.h"
#include "state_interface.h"
#include <chrono>

namespace State {

class MainDescent : public IState {
  public:
    /**
     * @param LANDED_TIME_INTERVAL time between each landed check, in
     * milliseconds
     * @param LANDED_CHECKS number of checks to perform
     * @param LANDED_VELOCITY maximum velocity to be considered as landed
     */
    MainDescent(StateId landed_state, long LANDED_TIME_INTERVAL,
                int LANDED_CHECKS, float LANDED_VELOCITY)
        : LANDED_TIME_INTERVAL_(LANDED_TIME_INTERVAL),
          LANDED_CHECKS_(LANDED_CHECKS), LANDED_VELOCITY_(LANDED_VELOCITY),
          landed_state_(landed_state) {}

    /**
     * @brief Return the assigned enumeration code.
     * @return Enumeration code.
     */
    StateId getStateEnum(void) { return StateId::MAIN_DESCENT; }

    /**
     * @brief Return the next state, based on input data (mostly from filtered
     * sensor data)
     * @return State enumeration code, to be passed into the std::map between
     * codes and used states. Note that the returned code may be the same state.
     */
    StateId getNewState(const StateInput &input,
                        StateAuxilliaryInfo &state_aux);

  private:
    const std::chrono::milliseconds LANDED_TIME_INTERVAL_;
    const uint8_t LANDED_CHECKS_;
    const float LANDED_VELOCITY_;
    const StateId landed_state_;
};

} // namespace State
