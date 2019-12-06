#ifndef STATE_INTERFACE_H
#define STATE_INTERFACE_H

#include "state_input_struct.h"

enum class State{
    STANDBY,
    ARMED,
    POWERED_ASCENT,
    PRE_AIR_START_COAST_TIMED,
    ASCENT_TO_APOGEE,
    PRESSURE_DELAY,
    DROGUE_DESCENT,
    MAIN_DESCENT,
    LANDED,
    WINTER_CONTINGENCY
};


class IState {
public:
    /*
    * @brief Return the assigned enumeration code based in
    * @return Enumeration.
    */
    virtual State getStateEnum(void) = 0;

    /*
    * @brief Retern the next state, based on input data (mostly from filtered sensor data)
    */
    virtual State getNewState(StateInput data) = 0;
}

#endif
