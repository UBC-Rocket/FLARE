#ifndef STATES__ASCENT_TO_APOGEE_H
#define STATES__ASCENT_TO_APOGEE_H

/*Includes------------------------------------------------------------*/
#include "state_interface.h"
#include "state_input_struct.h"

namespace State {

    class AscentToApogee : public IState {
    public:
        AscentToApogee(uint8_t const APOGEE_CHECKS, uint8_t const MACH_LOCK_CHECKS, float MACH_LOCK_VELOCITY) :
            M_APOGEE_CHECKS(APOGEE_CHECKS),
            M_MACH_LOCK_CHECKS(MACH_LOCK_CHECKS),
            M_MACH_LOCK_VELOCITY(MACH_LOCK_VELOCITY) {}

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
        uint8_t M_APOGEE_CHECKS;
        uint8_t M_MACH_LOCK_CHECKS;
        float M_MACH_LOCK_VELOCITY;
    };

}
#endif

