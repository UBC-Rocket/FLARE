/* Main Arduino Sketch */
/*
VERY IMPORTANT PLEASE READ ME! VERY IMPORTANT PLEASE READ ME! VERY IMPORTANT
PLEASE READ ME!

                 uuuuuuu
             uu$$$$$$$$$$$uu
          uu$$$$$$$$$$$$$$$$$uu
         u$$$$$$$$$$$$$$$$$$$$$u
        u$$$$$$$$$$$$$$$$$$$$$$$u
       u$$$$$$$$$$$$$$$$$$$$$$$$$u
       u$$$$$$$$$$$$$$$$$$$$$$$$$u
       u$$$$$$"   "$$$"   "$$$$$$u
       "$$$$"      u$u       $$$$"
        $$$u       u$u       u$$$
        $$$u      u$$$u      u$$$
         "$$$$uu$$$   $$$uu$$$$"
          "$$$$$$$"   "$$$$$$$"
            u$$$$$$$u$$$$$$$u
             u$"$"$"$"$"$"$u
  uuu        $$u$ $ $ $ $u$$       uuu
 u$$$$        $$$$$u$u$u$$$       u$$$$
  $$$$$uu      "$$$$$$$$$"     uu$$$$$$
u$$$$$$$$$$$uu    """""    uuuu$$$$$$$$$$
$$$$"""$$$$$$$$$$uuu   uu$$$$$$$$$"""$$$"
 """      ""$$$$$$$$$$$uu ""$"""
           uuuu ""$$$$$$$$$$uuu
  u$$$uuu$$$$$$$$$uu ""$$$$$$$$$$$uuu$$$
  $$$$$$$$$$""""           ""$$$$$$$$$$$"
   "$$$$$"                      ""$$$$""
     $$$"                         $$$$"

In order to successfully poll the GPS, the serial RX buffer size must be
increased. This needs to be done on the computer used for compilation. This can
be done by navigating to the following path in the Arduino contents folder: On
Mac: Got to the Applications folder, right click on the Arduino app, select Show
Package Contents, then navigate to
‎⁨Contents⁩/⁨Java⁩/⁨hardware⁩/⁨teensy⁩/⁨avr⁩/⁨cores⁩/⁨teensy3⁩/serial1.c
On Windows: [user_drive]\Program Files
(x86)\Arduino\hardware\teensy\avr\cores\teensy3\serial1.c

On line 43 increase SERIAL1_RX_BUFFER_SIZE from 64 to 1024

THIS MUST BE DONE ON THE COMPUTER USED TO COMPILE THE CODE!!!

VERY IMPORTANT PLEASE READ ME! VERY IMPORTANT PLEASE READ ME! VERY IMPORTANT
PLEASE READ ME!
*/

/* @file    avionics.ino
 * @author  UBC Rocket Avionics 2018/2019
 * @description The main arduino sketch that controls the flow
 *     of our 30K sensors team code.
 *
 * @section LICENSE
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * Distributed as-is; no warranty is given.
 */

/* Includes------------------------------------------------------------*/
#include "HAL/pin_util.h"
#include "HAL/time.h"

#include "scheduler.hpp"

#include "radio.h"
#include "rocket.h"
#include "state_input_struct.h"
#include "state_interface.h"

#include "config.h"
#include "env_config.h"

/* Errors---------------------------------------------------------------*/
// #if defined NOSECONE && defined BODY
// #error Only one of NOSECONE and BODY may be defined!
// #elif !(defined NOSECONE || defined BODY)
// #error Define one of NOSECONE or BODY!
// #endif

// #if defined POW && defined SERVO
// #error Only one of POW and SERVO may be defined!
// #elif !(defined POW || defined SERVO)
// #error Define one of POW or SERVO!
// #endif

// #if defined TESTING
// #warning TESTING is defined! Do not fly this code
// #endif
// #if defined GROUND_TEST
// #warning GROUND_TEST is defined! Do not fly this code
// #endif

/* Variables ========================================================== */

/* Tasks ============================================================== */

/**
 * @brief Main sensor read, state estimation, run state machine, log data loop
 */
class ReadEvalLog {
  private:
    Rocket &rocket_;
    CommandReceiver command_receiver;
    // TODO - see if state_aux is actually used / wheter it should be used
    StateAuxilliaryInfo state_aux; // Output info from states

    constexpr static unsigned int NOMINAL_POLLING_TIME_INTERVAL = 50; // ms
    // MainDescent state updates this to 5000 ms - see main_descent.cpp

    void run() {
        auto &state_machine = rocket_.state_machine;
        auto &init_status = rocket_.init_status;
        auto &sensors = rocket_.sensors;
        auto &calc = rocket_.calc;
        auto &datalog = rocket_.datalog;

        // ensures that if state somehow gets accidentally changed, state
        // reverts to aborted
        if (init_status == RocketStatus::CRITICAL_FAILURE) {
            state_machine.abort();
        }

        StateId state = state_machine.getState();
        StateInput state_input;

        rocket_.sensors.poll();
        calc.calculateValues(state, state_input, sensors.last_poll_time());
        state_machine.update(state_input, state_aux);
        datalog.logData(Hal::tpoint_to_uint(sensors.last_poll_time()), sensors,
                        state, calc.altitude(), calc.getBaseAltitude());

        Radio::forwardCommand(command_receiver);
    }

  public:
    ReadEvalLog(Rocket &rocket) : rocket_(rocket), command_receiver(rocket) {}
    static void run(void *self) {
        reinterpret_cast<ReadEvalLog *>(self)->run();
    }
};

/**
 * @brief Responsible for periodically sending bulk sensor data through radio
 */
class RadioTxBulk {
  public:
    static void run(void *self) {
        reinterpret_cast<RadioTxBulk *>(self)->run();
    }

    RadioTxBulk(Rocket &rkt) : rocket(rkt) {}
    static constexpr Hal::ms freq{500};

  private:
    void run() {
        StateId state = rocket.state_machine.getState();
        Radio::sendBulkSensor(
            Hal::tpoint_to_uint(rocket.sensors.last_poll_time()),
            rocket.calc.altitude(), rocket.sensors.accelerometer,
            rocket.sensors.imuSensor, rocket.sensors.gps,
            static_cast<uint8_t>(state));
    }

    Rocket &rocket;
};
constexpr Hal::ms RadioTxBulk::freq; // C++ is weird sometimes

/**
 * @brief Responsbile for blinking the status LED on non-critical failure.
 */
class LEDBlinker {
  private:
    static bool led_on;

  public:
    static void toggle(void *) {
        if (led_on)
            Hal::digitalWrite(Pin::BUILTIN_LED, Hal::PinDigital::HIGH);
        else
            Hal::digitalWrite(Pin::BUILTIN_LED, Hal::PinDigital::LOW);

        led_on = !led_on;
    }

    static constexpr Hal::ms freq{500};
};
constexpr Hal::ms LEDBlinker::freq;
bool LEDBlinker::led_on = true;

/**
 * @brief Helper function that makes things less verbose; basically saves the
 * static_cast call
 */
void registerTask(TaskID id, Scheduler::Task task, bool repeat = true,
                  bool early = false) {
    Scheduler::registerTask(static_cast<int>(id), task, repeat, early);
}

/* Main function ====================================================== */
int main(void) {
    // Before anything else there's some environment specific setup to be done
    env_initialize();
    initPins();

/* Setup all UART comms */
// Serial comms to computer
#ifdef TESTING
    SerialUSB.begin(9600);
    while (!SerialUSB) {
    }
    SerialUSB.println("Initializing...");
#endif

    Radio::initialize();
    Rocket rocket;
    // Logically, these are all unrelated variables - but to allow the command
    // receiver to function, they need to be coalesced into one POD struct.
    auto &state_machine = rocket.state_machine;
    auto &init_status = rocket.init_status;
    auto &sensors = rocket.sensors;
    auto &ignitors = rocket.ignitors;

    StateInput state_input;
    StateAuxilliaryInfo state_aux; // Output info from states

    if (init_status == RocketStatus::CRITICAL_FAILURE) {
        state_machine.abort();
    }

    Radio::sendStatus(Hal::millis(), init_status, sensors, ignitors);

    /* Register all tasks */
    typedef Scheduler::Task Task;

    ReadEvalLog read_eval_logger(rocket);
    Task read_eval_log(ReadEvalLog::run, &read_eval_logger, Hal::ms(50));
    registerTask(TaskID::ReadEvalLog, read_eval_log);

    RadioTxBulk radio_txer(rocket);
    Task radio_tx(RadioTxBulk::run, &radio_txer, RadioTxBulk::freq);
    registerTask(TaskID::RadioTxBulk, radio_tx);

    Task led_blink(LEDBlinker::toggle, nullptr, LEDBlinker::freq);

    switch (init_status) {
    case RocketStatus::NOMINAL:
        Hal::digitalWrite(Pin::BUILTIN_LED, Hal::PinDigital::LOW);
        break;
    case RocketStatus::NONCRITICAL_FAILURE:
        registerTask(TaskID::LEDBlinker, led_blink);
        break;
    case RocketStatus::CRITICAL_FAILURE:
        Hal::digitalWrite(Pin::BUILTIN_LED, Hal::PinDigital::LOW);
        break;
    }

    Scheduler::run();
}
