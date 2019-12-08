#include "states/pre_air_start_coast_timed.h"
#include "Eigen/Geometry"

/*
https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation#Using_quaternion_as_rotations
Identity rotation is simply theta = 0 --> imaginary part goes to zero, real part is 1
*/
constexpr IDENTITY_QUATERNION = Eigen::Quaternion(1, 0, 0, 0)

StateId State::PreAirStartCoastTimed::flightNominal(const StateInput &input){
    //Basic checks
    if (input.altitude > M_MAX_ACCEPT_ALTITUDE
        || input.altitude < M_MIN_ACCEPT_ALTITUDE
        || input.accel_ground(x) > M_MAX_ACCEPT_VERTICAL_ACCEL
        || input.accel_ground(x) < M_MIN_ACCEPT_VERTICAL_ACCEL)
        || !input.ignitor_good {
        return false;
    }

    //Orientation is a little harder
    if (input.orientation.angularDistance(IDENTITY_QUATERNION) > M_MAX_ACCEPT_ANGLE_FROM_VERTICAL){
        return false;
    }

    return true;
}


StateId State::PreAirStartCoastTimed::getNewState(const StateInput &input, StateAuxilliaryInfo &state_aux){
    static uint32_t start_time = millis();
    if (millis() - start_time > M_DELAY_TIME_MS){
        state_aux.is_abort = !flightNominal(input);
        return m_post_ascent_id;
    }
    return StateId::PRE_AIR_START_COAST_TIMED;
}

