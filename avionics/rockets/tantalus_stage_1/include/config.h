#ifndef CONFIG_H_41E0CC504D364079AC42AF610BBE6F7A
#define CONFIG_H_41E0CC504D364079AC42AF610BBE6F7A

#include <unordered_map> //for std::unordered_map (hash map)

#include "state_constants_config.h"

#include "states/standby.h"
#include "states/armed.h"
#include "states/powered_ascent.h"
#include "states/pre_air_start_coast_timed.h"
#include "states/mach_lock.h"
#include "states/ascent_to_apogee.h"
#include "states/pressure_delay.h"
#include "states/drogue_descent.h"
#include "states/main_descent.h"
#include "states/landed.h"

/* States */
static State::Standby state_standby = State::Standby(StateId::POWERED_ASCENT, STANDBY_LAUNCH_CHECKS, LAUNCH_THRESHOLD);

static State::Armed state_armed = State::Armed(StateId::POWERED_ASCENT, ARMED_LAUNCH_CHECKS, LAUNCH_THRESHOLD);

static State::PoweredAscent state_powered_ascent = State::PoweredAscent(StateId::PRE_AIR_START_COAST_TIMED, MOTOR_BURNOUT_CHECKS);

static State::PreAirStartCoastTimed state_prestage_coast = State::PreAirStartCoastTimed(
        POST_BURNOUT_COAST_TIME,
        PRESTAGE_MAX_ACCEPT_ALTITUDE,
        PRESTAGE_MIN_ACCEPT_ALTITUDE,
        PRESTAGE_MAX_ACCEPT_VERTICAL_ACCEL,
        PRESTAGE_MIN_ACCEPT_VERTICAL_ACCEL,
        PRESTAGE_MAX_ACCEPT_ANGLE_FROM_VERTICAL
    );

static State::AscentToApogee state_coast = State::AscentToApogee(APOGEE_CHECKS, MACH_LOCK_CHECKS, MACH_LOCK_VELOCITY_THRESHOLD);

static State::MachLock state_mach_lock = State::MachLock(MACH_UNLOCK_CHECKS, MACH_UNLOCK_VELOCITY_THRESHOLD);

static State::PressureDelay state_pres_delay = State::PressureDelay(APOGEE_PRESSURE_DELAY);

static State::DrogueDescent state_drogue = State::DrogueDescent(MAIN_DEPLOY_ALTITUDE, MAIN_DEPLOY_CHECKS);

static State::MainDescent state_main = State::MainDescent(std::chrono::milliseconds(LAND_CHECK_TIME_INTERVAL), LAND_CHECKS, LAND_VELOCITY_THRESHOLD);

static State::Landed state_landed = State::Landed();

static std::unordered_map<StateId, IState*> state_hash_map = {
    {StateId::STANDBY, &state_standby},
    {StateId::ARMED, &state_armed},
    {StateId::POWERED_ASCENT, &state_powered_ascent},
    {StateId::PRE_AIR_START_COAST_TIMED, &state_prestage_coast},
    {StateId::ASCENT_TO_APOGEE, &state_coast},
    {StateId::MACH_LOCK, &state_mach_lock},
    {StateId::PRESSURE_DELAY, &state_pres_delay},
    {StateId::DROGUE_DESCENT, &state_drogue},
    {StateId::MAIN_DESCENT, &state_main},
    {StateId::LANDED, &state_landed}
};


static StateId state = StateId::STANDBY;

/* TODO - MAKE THIS NOT A CONSTANT*/
#include "state_input_struct.h"

static StateInput state_input;
static StateAuxilliaryInfo state_aux;


/* Sensors */
#include "sensors-interface.h"
#include "sensors/accelerometer.h"
#include "sensors/barometer.h"
#include "sensors/GPS.h"
#include "sensors/IMU.h"
#include "sensors/temperature.h"
static Accelerometer accelerometer;
static Barometer barometer;
static GPS gps(SerialGPS);
static IMU imuSensor;
static Temperature temperature;

std::vector<std::reference_wrapper<ISensor> > sensors {
    accelerometer,
    barometer,
    gps,
    imuSensor,
    temperature
};

/* Parachute */
#include "hw-interface.h"
#include "hardware/ignitor.h"

// TODO: Add ignitor to hardware vector with proper pin initialization

std::vector<std::reference_wrapper<IParachute> > hardware;

/* Calculator */
#include "calculations.h"

Calculator calc(&barometer, &imuSensor);


#endif
