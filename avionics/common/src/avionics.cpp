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
#include <functional> //for std::reference_wrapper
#include <string.h>

#include "HAL/pin_util.h"
#include <HAL/port_impl.h> // <> vs "" for HAL ??

#include "CSVWriteImpl.h"
#include "CSVwrite.h"
#include "buzzer.h"
#include "calculations.h"
#include "cameras.h"
#include "gpio.h"
#include "ignitor_collection.h"
#include "options.h"
#include "radio.h"
#include "sensor_collection.h"
#include "state_machine.h"
#include "status.h"

#include "config.h"
#include "env_config.h"

/* Errors---------------------------------------------------------------*/
#if defined NOSECONE && defined BODY
#error Only one of NOSECONE and BODY may be defined!
#elif !(defined NOSECONE || defined BODY)
#error Define one of NOSECONE or BODY!
#endif

#if defined POW && defined SERVO
#error Only one of POW and SERVO may be defined!
#elif !(defined POW || defined SERVO)
#error Define one of POW or SERVO!
#endif

#if defined TESTING
#warning TESTING is defined! Do not fly this code
#endif
#if defined GROUND_TEST
#warning GROUND_TEST is defined! Do not fly this code
#endif

/* Variables------------------------------------------------------------*/

/* Functions------------------------------------------------------------*/
void blinkStatusLED(RocketStatus);

int main(void) {
    // Before anything else there's some environment specific setup to be done
    env_initialize();

    constexpr unsigned int LANDED_POLLING_TIME_INTERVAL = 5000; // ms
    constexpr unsigned int NOMINAL_POLLING_TIME_INTERVAL = 50;  // ms

    static Buzzer buzzer;
    Camera cam(Hal::SerialInst::Camera);

    initPins();

/* Setup all UART comms */
// Serial comms to computer
#ifdef TESTING
    SerialUSB.begin(9600);
    while (!SerialUSB) {
    }
    SerialUSB.println("Initializing...");
#endif

    /* init sensors*/
    SensorCollectionPtr sensors_ptr = getSensors();
    SensorCollection &sensors = *sensors_ptr;

    /* init ignitors*/
    IgnitorCollectionPtr ignitors_ptr = getIgnitors();
    IgnitorCollection &ignitors = *ignitors_ptr;

    /* init log file */
    constexpr char LOG_FILE_NAME[] = "datalog.csv";
    CSVWrite<CSVWriteImpl> datalog(LOG_FILE_NAME);

    RocketStatus statusOfInit = collectStatus(sensors, ignitors);
    displayStatus(statusOfInit, buzzer);

    RadioController radio{Hal::SerialInst::Radio};

    Calculator calc(sensors);

    StateMachine state_machine;
    StateId state = state_machine.getState();

    // Timing
    Hal::t_point timestamp;
    Hal::t_point old_time = Hal::now_ms(); // ms
    Hal::t_point new_time;                 // ms
    uint32_t new_time_int;
    Hal::ms time_interval(NOMINAL_POLLING_TIME_INTERVAL); // ms
    Hal::t_point radio_old_time = Hal::now_ms();
    Hal::ms radio_t_interval(500); // ms //TODO - make 500 a constant somewhere
    radio.sendStatus(old_time.time_since_epoch().count(), statusOfInit, sensors,
                     ignitors);

    float altitude = 0;

    auto command_reciever = [&new_time_int, &statusOfInit, &sensors, &ignitors,
                             &radio, &state_machine, &altitude,
                             &state](uint8_t command) {
        switch (command) {
        case 'P':
            radio.sendStatus(new_time_int, statusOfInit, sensors, ignitors);
            break;
        case 'A':
            state_machine.arm();
            break;
        case 'D':
            state_machine.disarm();
            break;
        case 0x30:
            radio.sendBulkSensor(new_time_int, altitude, sensors.accelerometer,
                                 sensors.imuSensor, sensors.gps,
                                 static_cast<uint8_t>(state));
        default:
            break;
        }
    };
    for (;;) {
        new_time = Hal::now_ms();
        new_time_int =
            static_cast<uint32_t>(timestamp.time_since_epoch().count());

        // makes sure that even if it does somehow get accidentally changed,
        // it gets reverted
        if (statusOfInit == RocketStatus::CRITICAL_FAILURE) {
            state_machine.abort();
        }

        radio.listenAndAct<decltype(command_reciever)>(command_reciever);

        state = state_machine.getState(); // convenience

        // Polling time intervals need to be variable, since in LANDED
        // there's a lot of data that'll be recorded
        if (state == StateId::LANDED)
            time_interval = Hal::ms(LANDED_POLLING_TIME_INTERVAL);
        else
            time_interval = Hal::ms(NOMINAL_POLLING_TIME_INTERVAL);

        // Core functionality of rocket - take data, process it,
        // run the state machine, and log the data
        if ((new_time - old_time) >= time_interval) {
            old_time = new_time;

            sensors.poll(timestamp);
            calc.calculateValues(state, state_input, new_time);
            altitude = state_input.altitude;

            state_machine.update(state_input, state_aux);
            datalog.logData(new_time_int, sensors, state, altitude,
                            calc.getBaseAltitude());
        }

        if (new_time - radio_old_time >= radio_t_interval) {
            radio_old_time = new_time;
            radio.sendBulkSensor(new_time_int, altitude, sensors.accelerometer,
                                 sensors.imuSensor, sensors.gps,
                                 static_cast<uint8_t>(state));
        }
        // LED blinks in non-critical failure
        blinkStatusLED(statusOfInit);

#ifdef TESTING
        Hal::sleep_ms(1000); // So you can actually read the serial output
#endif
#ifdef SLEEP_BECAUSE_NATIVE_CONFIGURATION
        Hal::sleep_ms((old_time + time_interval - new_time).count());
#endif
    }
}

/**
 * @brief  Helper function for LED blinking
 * @return None
 */
inline void blinkStatusLED(RocketStatus statusOfInit) {
    static Hal::t_point init_st_old_time = Hal::now_ms();
    static const Hal::ms init_st_time_interval(500);
    static bool init_st_indicator = false;

    if (statusOfInit == RocketStatus::NONCRITICAL_FAILURE &&
        Hal::now_ms() - init_st_old_time > init_st_time_interval) {
        init_st_old_time = Hal::now_ms();
        init_st_indicator = !init_st_indicator;

        if (init_st_indicator)
            Hal::digitalWrite(Pin::BUILTIN_LED, Hal::PinDigital::HIGH);
        else
            Hal::digitalWrite(Pin::BUILTIN_LED, Hal::PinDigital::LOW);
    }
}
