#ifndef STATES__POWERED_ASCENT_H
#define STATES__POWERED_ASCENT_H

/*Includes------------------------------------------------------------*/
#include "state_interface.h"
#include "state_input_struct.h"

namespace State {

    class PoweredAscent : IState {
    public:
        PoweredAscent(StateId const post_ascent_id, int const BURNOUT_CHECKS) : m_post_ascent_id(post_ascent_id),
            M_BURNOUT_CHECKS(BURNOUT_CHECKS) {}

        /*
        * @brief Return the assigned enumeration code.
        * @return Enumeration code.
        */
        StateId getStateEnum(void){ return StateId::POWERED_ASCENT; }

        /*
        * @brief Return the next state, based on input data (mostly from filtered sensor data)
        * @return State enumeration code, to be passed into the std::map between codes and used states. Note that the returned code may be the same state.
        */
        StateId getNewState(StateInput const &input, StateAuxilliaryInfo &state_aux);

    private:
        StateId m_post_ascent_id;
        int M_BURNOUT_CHECKS;
    }

}
#endif

