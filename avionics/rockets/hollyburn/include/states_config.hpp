#pragma once

#include <unordered_map> //for std::unordered_map (hash map)

#include "calculations.h"
#include "ignitor_collection.h"
#include "states/repeated_checks.hpp"

#include "states/ascent_to_apogee.h"
#include "states/landed.h"
#include "states/main_descent.h"
#include "states/pressure_delay.h"
#include "states/winter_contingency.h"

struct StateMachineConfig {
  private:
    /* Configuration constants */
    constexpr static int STANDBY_LAUNCH_CHECKS = 4;
    constexpr static int ARMED_LAUNCH_CHECKS = 4;
    constexpr static int MOTOR_BURNOUT_CHECKS = 4;
    constexpr static int MACH_LOCK_CHECKS = 10;
    constexpr static int MACH_UNLOCK_CHECKS = 10;
    constexpr static int APOGEE_CHECKS = 5;
    constexpr static int MAIN_DEPLOY_CHECKS = 5;

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
    State::Standby<StateId::ASCENT_TO_APOGEE, STANDBY_LAUNCH_CHECKS> standby;

    State::Armed<StateId::ASCENT_TO_APOGEE, ARMED_LAUNCH_CHECKS> armed{
        LAUNCH_THRESHOLD};

    State::AscentToApogee coast;

    State::MachLock<StateId::ASCENT_TO_APOGEE, MACH_UNLOCK_CHECKS> mach_lock{
        MACH_UNLOCK_VELOCITY_THRESHOLD};

    State::PressureDelay pres_delay =
        State::PressureDelay(StateId::DROGUE_DESCENT, APOGEE_PRESSURE_DELAY);

    State::DrogueDescent<StateId::MAIN_DESCENT, MAIN_DEPLOY_CHECKS> drogue;

    State::MainDescent main;

    State::Landed landed;

    State::WinterContingency contingency{};

  public:
    StateMachineConfig(const Calculator &calc, IgnitorCollection &ignitors,
                       Camera &camera)
        : standby(LAUNCH_THRESHOLD, camera),
          coast(StateId::PRESSURE_DELAY, StateId::MACH_LOCK, APOGEE_CHECKS,
                MACH_LOCK_CHECKS, MACH_LOCK_VELOCITY_THRESHOLD,
                ignitors.drogue),
          drogue(MAIN_DEPLOY_ALTITUDE, ignitors.main),
          main(StateId::LANDED, LAND_CHECK_TIME_INTERVAL, LAND_CHECKS,
               LAND_VELOCITY_THRESHOLD, calc) {}

    std::unordered_map<StateId, IState *> state_map = {
        {StateId::STANDBY, &standby},
        {StateId::ARMED, &armed},
        {StateId::ASCENT_TO_APOGEE, &coast},
        {StateId::MACH_LOCK, &mach_lock},
        {StateId::PRESSURE_DELAY, &pres_delay},
        {StateId::DROGUE_DESCENT, &drogue},
        {StateId::MAIN_DESCENT, &main},
        {StateId::LANDED, &landed},
        {StateId::WINTER_CONTINGENCY, &contingency}};

    StateId initial_state = StateId::STANDBY;
};
