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
#include "tasks/led_blinker.hpp"
#include "tasks/main_tasks.hpp"
#include "tasks/restart_camera.hpp"

#include "radio.h"
#include "rocket.h"
#include "status.h"

#include "config.h"
#include "env_config.h"

#include "log.hpp"
#include "options.h"

/**
 * @brief Helper function that makes things less verbose; basically saves the
 * static_cast call
 */
void registerTask(TaskID id, Scheduler::Task task, bool repeat = true,
                  bool early = false) {
    Scheduler::registerTask(static_cast<int>(id), task, repeat, early);
}

// added for tantalus lite version
// this really shouldnt be here but importing buzzer is very finnicky so this has been copied over
void buzz(long frequency, long length) {
    const Pin M_MELODY_PIN = Pin::BUZZER;
    long delayValue = 1000000 / frequency / 2; // delay between transitions
    // 1 000 000 microseconds, divided by the frequency, divided by 2 b/c
    // there are two phases to each cycle
    long numCycles = frequency * length / 1000; // #of cycles for proper timing
    // multiply frequency = cycles per second, by the number of seconds to
    // get the total number of cycles to produce
    for (long i = 0; i < numCycles; i++) { // for the calculated length of time
        Hal::digitalWrite(
            M_MELODY_PIN,
            Hal::PinDigital::HI); // write high to push out the diaphram
        Hal::sleep_us(delayValue);  // wait for the calculated delay value
        Hal::digitalWrite(
            M_MELODY_PIN,
            Hal::PinDigital::LO); // write low to pull back the diaphram
        Hal::sleep_us(delayValue); // wait for the calculated delay value
    }
    Hal::sleep_ms(100);
}

void singStartup() {
    const int NOTE_A4 = 440;
    const int NOTE_F4 = 349;
    const int NOTE_C5 = 523;
    const int NOTE_E5 = 659;
    const int NOTE_F5 = 698;
    const int NOTE_GS4 = 415;

#ifdef STAGE2
    buzz(NOTE_A4, 400);
    buzz(NOTE_A4, 400);    
    buzz(NOTE_A4, 400);
    buzz(NOTE_F4, 250);
    buzz(NOTE_C5, 100);  
    buzz(NOTE_A4, 400);
    buzz(NOTE_F4, 250);
    buzz(NOTE_C5, 100);
    buzz(NOTE_A4, 400);
#else
    buzz(NOTE_E5, 400);
    buzz(NOTE_E5, 400);
    buzz(NOTE_E5, 400);  
    buzz(NOTE_F5, 250);
    buzz(NOTE_C5, 100);
    buzz(NOTE_GS4, 400);
    buzz(NOTE_F4, 250);
    buzz(NOTE_C5, 100);
    buzz(NOTE_A4, 400);
#endif
}

/* Main function ====================================================== */
int main(void) {
    // Before anything else there's some environment specific setup to be done
    env_initialize();
    LOG_INFO("Everything is starting now");

    initPins();

/* Setup all UART comms */
// Serial comms to computer
#ifdef TESTING
    SerialUSB.begin(9600);
    while (!SerialUSB) {
    }
    SerialUSB.println("Initializing...");
#endif

    // Disabled for lite version
    // Radio::initialize();
    // LOG_INFO("Initialized radio");

    Rocket rocket;
    // Logically, these are all unrelated variables - but to allow the command
    // receiver to function, they need to be coalesced into one POD struct.
    auto &state_machine = rocket.state_machine;
    auto &init_status = rocket.init_status;
    // auto &sensors = rocket.sensors; unnecessary for lite version
    // auto &ignitors = rocket.ignitors;

    if (init_status == RocketStatus::CRITICAL_FAILURE) {
        LOG_ERROR("Critical failure; aborting in state machine");
        state_machine.abort();
    }

    // Disabled for lite version
    // Radio::sendStatus(Hal::millis(), init_status, sensors, ignitors);
    // LOG_INFO("Sent startup status");

    /* Register all tasks */
    typedef Scheduler::Task Task;

    ReadEvalLog read_eval_logger(rocket);
    Task read_eval_log(ReadEvalLog::run, &read_eval_logger, Hal::ms(50));
    registerTask(TaskID::ReadEvalLog, read_eval_log);

    // Disabled for lite version
    // Radio needs to be scheduled later; sensors need to be read first
    // RadioTxBulk radio_txer(rocket);
    // Task radio_tx(RadioTxBulk::run, &radio_txer, RadioTxBulk::freq);
    // Scheduler::preregisterTask(static_cast<int>(TaskID::RadioTxBulk), radio_tx,
    //                            true, false);
    // Scheduler::scheduleTask(Hal::now_ms() + Hal::ms(1),
    //                         static_cast<int>(TaskID::RadioTxBulk));

    Task led_blink(LEDBlinker::toggle, nullptr, LEDBlinker::freq);

    switch (init_status) {
    case RocketStatus::NOMINAL:
        Hal::digitalWrite(Pin::BUILTIN_LED, Hal::PinDigital::LO);
        singStartup();
        break;
    case RocketStatus::NONCRITICAL_FAILURE:
        registerTask(TaskID::LEDBlinker, led_blink);
        break;
    case RocketStatus::CRITICAL_FAILURE:
        Hal::digitalWrite(Pin::BUILTIN_LED, Hal::PinDigital::LO);
        break;
    }

    RestartCamera restart_camera_(rocket.cam);
    // This tasks sets its own reschedule interval (since the same task is run
    // both to start and stop the recording)
    Task restart_camera_task_(RestartCamera::togglePower, &restart_camera_,
                              Hal::ms{0});
    Scheduler::preregisterTask(static_cast<int>(TaskID::RestartCamera),
                               restart_camera_task_, true, false);

    LOG_INFO("Initialization done; starting scheduler");

    Scheduler::run();

    LOG_ERROR("Somehow finished all tasks; main executable exiting");
}
