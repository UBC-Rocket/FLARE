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
#include "HAL/pin_util.h"

/**
 * @brief Helper function that makes things less verbose; basically saves the
 * static_cast call
 */
void registerTask(TaskID id, Scheduler::Task task, bool repeat = true,
                  bool early = false) {
    Scheduler::registerTask(static_cast<int>(id), task, repeat, early);
}

// TODO: Move to buzzer.cpp
void buzzNote(int frequency, int length = 750, int pause = 50) {
    tone(static_cast<uint8_t>(Pin::BUZZER), frequency, length);
    Hal::sleep_ms(length + pause);
}

/* Main function ====================================================== */
int main(void) {
    // Initialize Arduino
    init();
    
    // Before anything else there's some environment specific setup to be done
    env_initialize();
    LOG_INFO("Everything is starting now");

    initPins();

/* Setup all UART comms */
// Serial comms to computer
#ifdef TESTING
    Serial.begin(9600);
    while (!Serial) {
        Hal::digitalWrite(Pin::BUILTIN_LED, Hal::PinDigital::HI);
        Hal::sleep_ms(100);
        Hal::digitalWrite(Pin::BUILTIN_LED, Hal::PinDigital::LO);
        Hal::sleep_ms(100);
    }
    Serial.println("Initializing...");
#endif

    Radio::initialize();
    LOG_INFO("Initialized radio");
    Rocket rocket;
    // Logically, these are all unrelated variables - but to allow the command
    // receiver to function, they need to be coalesced into one POD struct.
    auto &state_machine = rocket.state_machine;
    auto &init_status = rocket.init_status;
    auto &sensors = rocket.sensors;
    auto &ignitors = rocket.ignitors;

    if (init_status == RocketStatus::CRITICAL_FAILURE) {
        LOG_ERROR("Critical failure; aborting in state machine");
        state_machine.abort();
    }

    Radio::sendStatus(Hal::millis(), init_status, sensors, ignitors);
    LOG_INFO("Sent startup status");

    /* Register all tasks */
    typedef Scheduler::Task Task;

    ReadEvalLog read_eval_logger(rocket);
    Task read_eval_log(ReadEvalLog::run, &read_eval_logger, Hal::ms(50));
    registerTask(TaskID::ReadEvalLog, read_eval_log);

    // Radio needs to be scheduled later; sensors need to be read first
    RadioTxBulk radio_txer(rocket);
    Task radio_tx(RadioTxBulk::run, &radio_txer, RadioTxBulk::freq);
    Scheduler::preregisterTask(static_cast<int>(TaskID::RadioTxBulk), radio_tx,
                               true, false);
    Scheduler::scheduleTask(Hal::now_ms() + Hal::ms(1),
                            static_cast<int>(TaskID::RadioTxBulk));

    Task led_blink(LEDBlinker::toggle, nullptr, LEDBlinker::freq);

    switch (init_status) {
    case RocketStatus::NOMINAL:
    {
    #ifdef TESTING
        Serial.println("Nominal");
    #endif
        Hal::digitalWrite(Pin::BUILTIN_LED, Hal::PinDigital::LO);

        int A = 440;
        int Bf = 466;
        int B = 494;
        int C = 523;
        int Cs = 554;
        int D = 587;
        int Ds = 622;
        int E = 659;
        int F = 698;
        int Fs = 740;
        int G = 784;

    #ifdef STAGE_2
        buzzNote(C, 500);
        buzzNote(G, 500);
        buzzNote(G, 500);
        buzzNote(F, 250);
        buzzNote(E, 250);
        buzzNote(F, 250);
        buzzNote(E, 250);
        buzzNote(D, 500);
        buzzNote(D, 500);
    #else
        buzzNote(C, 250);
        buzzNote(D, 250);
        buzzNote(E, 500);
        buzzNote(D, 250);
        buzzNote(E, 250);
        buzzNote(F, 500);
        buzzNote(E, 250);
        buzzNote(C, 250);
        buzzNote(E, 500);
        buzzNote(D, 1000);
    #endif
        break;
    }
    case RocketStatus::NONCRITICAL_FAILURE:
    #ifdef TESTING
        Serial.println("Non-critical failure");
    #endif
        // registerTask(TaskID::LEDBlinker, led_blink);
        break;
    case RocketStatus::CRITICAL_FAILURE:
    #ifdef TESTING
        Serial.println("Critical failure");
    #endif
        Hal::digitalWrite(Pin::BUILTIN_LED, Hal::PinDigital::HI);
        break;
    }

    // RestartCamera restart_camera_(rocket.cam);
    // // This tasks sets its own reschedule interval (since the same task is run
    // // both to start and stop the recording)
    // Task restart_camera_task_(RestartCamera::togglePower, &restart_camera_,
    //                           Hal::ms{0});
    // Scheduler::preregisterTask(static_cast<int>(TaskID::RestartCamera),
    //                            restart_camera_task_, true, false);

    LOG_INFO("Initialization done; starting scheduler");

    Scheduler::run();

    LOG_ERROR("Somehow finished all tasks; main executable exiting");
}
