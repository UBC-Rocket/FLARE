#pragma once

#include <unordered_map> //for std::unordered_map (hash map)

#include "states/armed.h"
#include "states/ascent_to_apogee.h"
#include "states/drogue_descent.h"
#include "states/landed.h"
#include "states/mach_lock.h"
#include "states/main_descent.h"
#include "states/powered_ascent.h"
#include "states/pre_air_start_coast_timed.h"
#include "states/pressure_delay.h"
#include "states/standby.h"
#include "states/winter_contingency.h"

class StateMachine {
  private:
    /* Configuration constants */
    constexpr static int STANDBY_LAUNCH_CHECKS = 4;
    constexpr static int ARMED_LAUNCH_CHECKS = 4;
    constexpr static int MOTOR_BURNOUT_CHECKS = 4;
    constexpr static int MACH_LOCK_CHECKS = 10;
    constexpr static int MACH_UNLOCK_CHECKS = 10;
    constexpr static int APOGEE_CHECKS = 5;
    constexpr static int MAIN_DEPLOY_CHECKS = 5;

    constexpr static long POST_BURNOUT_COAST_TIME = 3000;           // ms
    constexpr static float PRESTAGE_MAX_ALTITUDE = 5000;            // m
    constexpr static float PRESTAGE_MIN_ALTITUDE = 1000;            // m
    constexpr static float PRESTAGE_MAX_VERTICAL_ACCEL = 20;        // m/s^2
    constexpr static float PRESTAGE_MIN_VERTICAL_ACCEL = -50;       // m/s^2
    constexpr static float PRESTAGE_MAX_ANGLE_FROM_VERTICAL = 1.57; // radians

    constexpr static float LAUNCH_THRESHOLD = 25;                // m
    constexpr static float MACH_LOCK_VELOCITY_THRESHOLD = 155;   // m/s
    constexpr static float MACH_UNLOCK_VELOCITY_THRESHOLD = 150; // m/s
    constexpr static float MAIN_DEPLOY_ALTITUDE = 488;           // m == 1500 ft

    constexpr static long LAND_CHECK_TIME_INTERVAL = 10000; // ms
    constexpr static int LAND_CHECKS = 6;
    constexpr static float LAND_VELOCITY_THRESHOLD = 4;

    constexpr static long APOGEE_PRESSURE_DELAY = 3000;
    constexpr static long LANDING_TIME_INTERVAL = 10000;
    constexpr static long TOGGLE_CAMERA_INTERVAL = 200;

    /* States */
    State::Standby standby = State::Standby(
        StateId::POWERED_ASCENT, STANDBY_LAUNCH_CHECKS, LAUNCH_THRESHOLD);

    State::Armed armed = State::Armed(StateId::POWERED_ASCENT,
                                      ARMED_LAUNCH_CHECKS, LAUNCH_THRESHOLD);

    State::PoweredAscent powered_ascent = State::PoweredAscent(
        StateId::PRE_AIR_START_COAST_TIMED, MOTOR_BURNOUT_CHECKS);

    State::PreAirStartCoastTimed prestage_coast = State::PreAirStartCoastTimed(
        POST_BURNOUT_COAST_TIME, PRESTAGE_MAX_ALTITUDE, PRESTAGE_MIN_ALTITUDE,
        PRESTAGE_MAX_VERTICAL_ACCEL, PRESTAGE_MIN_VERTICAL_ACCEL,
        PRESTAGE_MAX_ANGLE_FROM_VERTICAL);

    State::AscentToApogee coast = State::AscentToApogee(
        APOGEE_CHECKS, MACH_LOCK_CHECKS, MACH_LOCK_VELOCITY_THRESHOLD);

    State::MachLock mach_lock =
        State::MachLock(MACH_UNLOCK_CHECKS, MACH_UNLOCK_VELOCITY_THRESHOLD);

    State::PressureDelay pres_delay =
        State::PressureDelay(APOGEE_PRESSURE_DELAY);

    State::DrogueDescent drogue =
        State::DrogueDescent(MAIN_DEPLOY_ALTITUDE, MAIN_DEPLOY_CHECKS);

    State::MainDescent main = State::MainDescent(
        LAND_CHECK_TIME_INTERVAL, LAND_CHECKS, LAND_VELOCITY_THRESHOLD);

    State::Landed landed = State::Landed();

    State::WinterContingency contingency{};

    std::unordered_map<StateId, IState *> hash_map = {
        {StateId::STANDBY, &standby},
        {StateId::ARMED, &armed},
        {StateId::POWERED_ASCENT, &powered_ascent},
        {StateId::PRE_AIR_START_COAST_TIMED, &prestage_coast},
        {StateId::ASCENT_TO_APOGEE, &coast},
        {StateId::MACH_LOCK, &mach_lock},
        {StateId::PRESSURE_DELAY, &pres_delay},
        {StateId::DROGUE_DESCENT, &drogue},
        {StateId::MAIN_DESCENT, &main},
        {StateId::LANDED, &landed},
        {StateId::WINTER_CONTINGENCY, &contingency}};

    StateId current_state = StateId::STANDBY;

  public:
    void update(const StateInput state_input, StateAuxilliaryInfo state_aux) {
        current_state =
            hash_map[current_state]->getNewState(state_input, state_aux);
    }

    /**
     * Set the state machine to a failsafe state
     */
    void abort() { current_state = StateId::WINTER_CONTINGENCY; }

    const StateId &getState() { return current_state; }
};
