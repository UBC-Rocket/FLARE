#ifndef STATES__PRE_AIR_START_COAST_TIMED_H
#define STATES__PRE_AIR_START_COAST_TIMED_H

/*Includes------------------------------------------------------------*/
#include "state_interface.h"
#include "state_input_struct.h"

namespace State {

    class PreAirStartCoastTimed : IState {
    public:
        /**
         * @brief Constructor. Altitudes are in metres above ground level (AGL), accelerations in m/s^2. Angle is in radians.
         */
        PreAirStartCoastTimed(
            uint32_t const DELAY_TIME_MS,
            float MAX_ACCEPT_ALTITUDE,
            float MIN_ACCEPT_ALTITUDE,
            float MAX_ACCEPT_VERTICAL_ACCEL,
            float MIN_ACCEPT_VERTICAL_ACCEL,
            float MAX_ACCEPT_ANGLE_FROM_VERTICAL) :
            M_MAX_ACCEPT_ALTITUDE(MAX_ACCEPT_ALTITUDE),
            M_MIN_ACCEPT_ALTITUDE(MIN_ACCEPT_ALTITUDE),
            M_MAX_ACCEPT_VERTICAL_ACCEL(MAX_ACCEPT_VERTICAL_ACCEL),
            M_MIN_ACCEPT_VERTICAL_ACCEL(MIN_ACCEPT_VERTICAL_ACCEL),
            M_MAX_ACCEPT_ANGLE_FROM_VERTICAL(MAX_ACCEPT_ANGLE_FROM_VERTICAL) {}

        /**
        * @brief Return the assigned enumeration code.
        * @return Enumeration code.
        */
        StateId getStateEnum(void){ return StateId::PRE_AIR_START_COAST_TIMED; }

        /*
        * @brief Return the next state, based on input data (mostly from filtered sensor data)
        * @return State enumeration code, to be passed into the std::map between codes and used states. Note that the returned code may be the same state.
        */
        StateId getNewState(const StateInput &input, StateAuxilliaryInfo &state_aux);

    private:
        uint32_t M_DELAY_TIME_MS;
        float M_MAX_ACCEPT_ALTITUDE;
        float M_MIN_ACCEPT_ALTITUDE;
        float M_MAX_ACCEPT_VERTICAL_ACCEL;
        float M_MIN_ACCEPT_VERTICAL_ACCEL;
        float M_MAX_ACCEPT_ANGLE_FROM_VERTICAL;

        /*
        * @brief Does the heavy lifting of checking whether flight is nominal
        * @param &input Reference to the input data structure
        * @return boolean true if everything is fine; false if something is not right
        */
        bool flightNominal(const StateInput &input);
    }

}
#endif

