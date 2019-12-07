#ifndef STATES__PRESSURE_DELAY_H
#define STATES__PRESSURE_DELAY_H

/*Includes------------------------------------------------------------*/
#include "state_interface.h"
#include "state_input_struct.h"

namespace State {

    class PressureDelay : IState {
    public:
        PressureDelay();

        /*
        * @brief Return the assigned enumeration code.
        * @return Enumeration code.
        */
        StateId getStateEnum(void){ return State::PressureDelay; }

        /*
        * @brief Return the next state, based on input data (mostly from filtered sensor data)
        * @return State enumeration code, to be passed into the std::map between codes and used states. Note that the returned code may be the same state.
        */
        StateId getNewState(StateInput data);
    }

}
#endif

