#ifndef STATES__ASCENT_TO_APOGEE_H
#define STATES__ASCENT_TO_APOGEE_H

/*Includes------------------------------------------------------------*/
#include "state_interface.h"
#include "state_input_struct.h"

namespace State {

    class AscentToApogee : IState {
    public:
        AscentToApogee(int8_t const apogee_checks) :
            M_APOGEE_CHECKS(apogee_checks){}

        /*
        * @brief Return the assigned enumeration code.
        * @return Enumeration code.
        */
        StateId getStateEnum(void){ return StateId::ASCENT_TO_APOGEE; }

        /*
        * @brief Return the next state, based on input data (mostly from filtered sensor data)
        * @return State enumeration code, to be passed into the std::map between codes and used states. Note that the returned code may be the same state.
        */
        StateId getNewState(const StateInput &input, StateAuxilliaryInfo &state_aux);

    private:
        int8_t M_APOGEE_CHECKS;
    }

}
#endif

