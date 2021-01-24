#pragma once

/*Includes------------------------------------------------------------*/
#include "state_input_struct.h"
#include "state_interface.h"

namespace State {

class MachLock : public IState {
  public:
    MachLock(StateId ascent_state, uint8_t MACH_UNLOCK_CHECKS,
             float MACH_UNLOCK_VELOCITY)
        : M_MACH_UNLOCK_CHECKS(MACH_UNLOCK_CHECKS),
          M_MACH_UNLOCK_VELOCITY(MACH_UNLOCK_VELOCITY),
          ascent_state_(ascent_state), unlock_checks(0) {}

    /**
     * @brief Return the assigned enumeration code.
     * @return Enumeration code.
     */
    StateId getStateEnum(void) { return StateId::MACH_LOCK; }

    /**
     * @brief Return the next state, based on input data (mostly from filtered
     * sensor data)
     * @return State enumeration code, to be passed into the std::map between
     * codes and used states. Note that the returned code may be the same state.
     */
    StateId getNewState(const StateInput &input,
                        StateAuxilliaryInfo &state_aux);

    void onEntry() override { unlock_checks = 0; }

  private:
    const StateId ascent_state_;
    const float M_MACH_UNLOCK_VELOCITY;
    const uint8_t M_MACH_UNLOCK_CHECKS;
    uint8_t unlock_checks;
};
} // namespace State
