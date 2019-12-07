#ifndef STATES__PRE_AIR_START_COAST_TIMED_H
#define STATES__PRE_AIR_START_COAST_TIMED_H

/*Includes------------------------------------------------------------*/
#include "state_interface.h"
#include "state_input_struct.h"

namespace State {

    class PreAirStartCoastTimed : IState {
    public:
        PreAirStartCoastTimed();

        /*
        * @brief Return the assigned enumeration code.
        * @return Enumeration code.
        */
        StateId getStateEnum(void){ return State::PreAirStartCoastTimed; }

        /*
        * @brief Return the next state, based on input data (mostly from filtered sensor data)
        * @return State enumeration code, to be passed into the std::map between codes and used states. Note that the returned code may be the same state.
        */
        StateId getNewState(StateInput data);
    }

}
#endif

