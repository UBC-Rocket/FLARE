#pragma once

/*Includes------------------------------------------------------------*/
#include "state_input_struct.h"
#include "state_interface.h"

namespace State {

class PoweredAscent : public IState {
  public:
    PoweredAscent(StateId const post_ascent_id, int const BURNOUT_CHECKS)
        : m_post_ascent_id(post_ascent_id), M_BURNOUT_CHECKS(BURNOUT_CHECKS) {}

    /*
     * @brief Return the assigned enumeration code.
     * @return Enumeration code.
     */
    StateId getStateEnum(void) { return StateId::POWERED_ASCENT; }

    /*
     * @brief Return the next state, based on input data (mostly from filtered
     * sensor data)
     * @return State enumeration code, to be passed into the std::map between
     * codes and used states. Note that the returned code may be the same state.
     */
    StateId getNewState(StateInput const &input,
                        StateAuxilliaryInfo &state_aux) {
        static uint8_t burnout_checks = 0;

        if (input.accel_rocket(0) < 0) {
            // accelerating backwards -- force is more drag than thrust
            burnout_checks++;
        } else {
            burnout_checks = 0;
        }

        if (burnout_checks >= M_BURNOUT_CHECKS) {
            return m_post_ascent_id;
        } else {
            return StateId::POWERED_ASCENT;
        }
    }

  private:
    StateId m_post_ascent_id;
    int M_BURNOUT_CHECKS;
};

} // namespace State
