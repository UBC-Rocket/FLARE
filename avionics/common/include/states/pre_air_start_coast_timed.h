#pragma once

/*Includes------------------------------------------------------------*/
#include "state_interface.h"

namespace State {

class PreAirStartCoastTimed : public IState {
  public:
    /**
     * @brief Constructor. Altitudes are in metres above ground level (AGL),
     * accelerations in m/s^2. Angle is in radians.
     */
    PreAirStartCoastTimed(uint32_t const DELAY_TIME_MS,
                          float MAX_ACCEPT_ALTITUDE, float MIN_ACCEPT_ALTITUDE,
                          float MAX_ACCEPT_VERTICAL_ACCEL,
                          float MIN_ACCEPT_VERTICAL_ACCEL,
                          float MAX_ACCEPT_ANGLE_FROM_VERTICAL)
        : DELAY_TIME_MS_(DELAY_TIME_MS),
          M_MAX_ACCEPT_ALTITUDE(MAX_ACCEPT_ALTITUDE),
          M_MIN_ACCEPT_ALTITUDE(MIN_ACCEPT_ALTITUDE),
          M_MAX_ACCEPT_VERTICAL_ACCEL(MAX_ACCEPT_VERTICAL_ACCEL),
          M_MIN_ACCEPT_VERTICAL_ACCEL(MIN_ACCEPT_VERTICAL_ACCEL),
          M_MAX_ACCEPT_ANGLE_FROM_VERTICAL(MAX_ACCEPT_ANGLE_FROM_VERTICAL),
          IDENTITY_QUATERNION(Eigen::Quaternionf().Identity()) {}

    /**
     * @brief Return the assigned enumeration code.
     * @return Enumeration code.
     */
    StateId getStateEnum(void) { return StateId::PRE_AIR_START_COAST_TIMED; }

    /**
     * @brief Return the next state, based on input data (mostly from filtered
     * sensor data)
     * @return State enumeration code, to be passed into the std::map between
     * codes and used states. Note that the returned code may be the same state.
     */
    StateId getNewState(Calculator const &) {
        typedef std::chrono::milliseconds ms;
        static auto start_time = Hal::now_ms();

        if (Hal::now_ms() - start_time > ms(DELAY_TIME_MS_)) {
            // TODO - report flight abort
            // state_aux.in_abort = !flightNominal(input);
            return StateId::ASCENT_TO_APOGEE;
        }
        return StateId::PRE_AIR_START_COAST_TIMED;
    }

  private:
    uint32_t DELAY_TIME_MS_;
    float M_MAX_ACCEPT_ALTITUDE;
    float M_MIN_ACCEPT_ALTITUDE;
    float M_MAX_ACCEPT_VERTICAL_ACCEL;
    float M_MIN_ACCEPT_VERTICAL_ACCEL;
    float M_MAX_ACCEPT_ANGLE_FROM_VERTICAL;

    /*
    https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation#Using_quaternion_as_rotations
    Identity rotation is simply theta = 0 --> imaginary part goes to zero, real
    part is 1
    */
    const Eigen::Quaternionf IDENTITY_QUATERNION;
    /*
     * @brief Does the heavy lifting of checking whether flight is nominal
     * @param &input Reference to the input data structure
     * @return boolean true if everything is fine; false if something is not
     * right
     */
    bool flightNominal(const StateInput &input) {
        // Basic checks
        if (input.altitude > M_MAX_ACCEPT_ALTITUDE ||
            input.altitude < M_MIN_ACCEPT_ALTITUDE ||
            input.accel_ground(0) > M_MAX_ACCEPT_VERTICAL_ACCEL ||
            input.accel_ground(0) < M_MIN_ACCEPT_VERTICAL_ACCEL ||
            !input.ignitor_good) {
            return false;
        }

        // Orientation is a little harder
        if (input.orientation.angularDistance(IDENTITY_QUATERNION) >
            M_MAX_ACCEPT_ANGLE_FROM_VERTICAL) {
            return false;
        }

        return true;
    }
};

} // namespace State
