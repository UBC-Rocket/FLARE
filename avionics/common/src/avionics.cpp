/* Main Arduino Sketch */
/*
VERY IMPORTANT PLEASE READ ME! VERY IMPORTANT PLEASE READ ME! VERY IMPORTANT PLEASE READ ME!

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

In order to successfully poll the GPS, the serial RX buffer size must be increased. This needs
to be done on the computer used for compilation. This can be done by navigating to the following
path in the Arduino contents folder:
On Mac: Got to the Applications folder, right click on the Arduino app, select Show Package Contents,
    then navigate to ‎⁨Contents⁩/⁨Java⁩/⁨hardware⁩/⁨teensy⁩/⁨avr⁩/⁨cores⁩/⁨teensy3⁩/serial1.c
On Windows: [user_drive]\Program Files (x86)\Arduino\hardware\teensy\avr\cores\teensy3\serial1.c

On line 43 increase SERIAL1_RX_BUFFER_SIZE from 64 to 1024

THIS MUST BE DONE ON THE COMPUTER USED TO COMPILE THE CODE!!!

VERY IMPORTANT PLEASE READ ME! VERY IMPORTANT PLEASE READ ME! VERY IMPORTANT PLEASE READ ME!
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

#include <Arduino.h>
#include <HardwareSerial.h>
#include <SD.h>
#include <SPI.h>
#include <i2c_t3.h>
#include <string.h>

#include "XBee.h"
#include "buzzer.h"
#include "calculations.h"
#include "cameras.h"
#include "gpio.h"
#include "options.h"
#include "radio.h"
#include "satcom.h"
#include "sensors.h"

#include "config.h"

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
static Status s_statusOfInit;

/* Radio */
static RadioController radio = RadioController(SerialRadio);

std::vector<std::reference_wrapper<IHardware> > hardware;  // Hardwares

/* Buzzer */
static Buzzer buzzer = Buzzer(MELODY_PIN);
/* Functions------------------------------------------------------------*/

// inline void sendSatcomMsg(FlightStates state, float GPS_data[], uint32_t timestamp);
void blinkStatusLED();

/**
  * @brief  The Arduino setup function
  * @param  None
  * @return None
  */
void setup() {
    int i; //used as loop variable
    initPins();

/* Setup all UART comms */
// Serial comms to computer
#ifdef TESTING
    SerialUSB.begin(9600);
    while (!SerialUSB) {
    }
    SerialUSB.println("Initializing...");
#endif

// Radio
// #ifdef TESTING
//     SerialUSB.println("Initializing radio");
// #endif

    // s_radio.setSerial(SerialRadio);
    // s_txPacket.setAddress64(s_gndAddr);


    // Comms to camera serial port
    SerialCamera.begin(CameraBaud);
    while (!SerialCamera) {
    }

    /*init I2C bus @ 400 kHz */
    Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_EXT, I2C_RATE_400);
    Wire.setDefaultTimeout(100000);  // 100ms

    /* Add all the sensors inside sensor vector */
    // sensors.push_back(accelerometer);
    // sensors.push_back(barometer);
    // sensors.push_back(gps);
    // sensors.push_back(imuSensor);
    // sensors.push_back(temperature);
    // sensors.push_back(thermocouple);

    /* init sensors and report status in many ways */
    initSensors(sensors, hardware, buzzer);

    /* TODO - make this not constant */
    state_input.ignitor_good = true;
    state_input.altitude = 5;
    state_input.velocity_vertical = 0;
    state_input.accel_ground = (Eigen::Vector3f() << 10.0f, 0.0f, 0.0f).finished();
    state_input.accel_rocket = (Eigen::Vector3f() << 10.0f, 0.0f, 0.0f).finished();
    state_input.orientation = Eigen::Quaternionf().Identity();

    //TODO - build this out
    // radioStatus(&s_radio, &s_txPacket, &s_statusOfInit);

}

/**
  * @brief  The Arduino loop function
  * @param  None
  * @return None
  */
void loop() {
    /* List of constants */
    static uint32_t timestamp;
    static unsigned long old_time = 0;  //ms
    static unsigned long new_time = 0;  //ms
    // unsigned long delta_time;
    static uint16_t time_interval = NOMINAL_POLLING_TIME_INTERVAL;  //ms

    // static unsigned long radio_old_time = 0;
    // static unsigned long radio_time_interval = 500;  //ms

    // float *battery_voltage, *acc_data, *bar_data, *temp_sensor_data, *IMU_data, *GPS_data, *thermocouple_data;

    static float altitude;

    // static FlightStates state = STANDBY;

    //makes sure that even if it does somehow get accidentally changed,
    //it gets reverted
    if (s_statusOfInit == Status::CRITICAL_FAILURE)
        state = StateId::WINTER_CONTINGENCY;


    radio.listenAndAct();
    // resolveRadioRx(&s_radio, &s_txPacket, GPS_data, &state, &s_statusOfInit);

#ifdef NOSECONE  // send satcom data
    sendSatcomMsg(state, GPS_data, timestamp);
#endif

    // Polling time intervals need to be variable, since in LANDED
    // there's a lot of data that'll be recorded
    if (state == StateId::LANDED)
        time_interval = LANDED_POLLING_TIME_INTERVAL;
    else
        time_interval = NOMINAL_POLLING_TIME_INTERVAL;

    //Core functionality of rocket - take data, process it,
    //run the state machine, and log the data
    new_time = millis();
    if ((new_time - old_time) >= time_interval) {
        old_time = new_time;

        pollSensors(&timestamp, sensors);

        calc.calculateValues(state, state_input, timestamp);
        altitude = state_input.altitude; //TODO - This is temporary fix for logData; should instead do something else.

        state = state_hash_map[state]->getNewState(state_input, state_aux);

        logData(timestamp, sensors, state, altitude, 0); //TODO - think some more about data logging and how it should mesh with calculations, and also get rid of baseline_pressure 
    }

    //LED blinks in non-critical failure
    blinkStatusLED();

#ifdef TESTING
    delay(1000);  //So you can actually read the serial output
#endif
}

/**
  * @brief  Helper function for satcom
  * @param  FlightStates state - flight state of the rocket
  * @param  float GPS_data[]
  * @param  float timestamp - time in ms
  * @return None
  */
// inline void sendSatcomMsg(FlightStates state, float GPS_data[], uint32_t timestamp) {
//     static bool mainDeploySatcomSent = false;
//     static int landedSatcomSentCount = 0;
//     static uint16_t satcomMsgOldTime = millis();

//     if (state == FINAL_DESCENT && !mainDeploySatcomSent) {
//         mainDeploySatcomSent = true;
//         SatComSendGPS(&timestamp, GPS_data);
//     } else if (state == LANDED && landedSatcomSentCount < NUM_SATCOM_SENDS_ON_LANDED && millis() - satcomMsgOldTime >= SATCOM_LANDED_TIME_INTERVAL) {
//         //sends Satcom total of NUM_SATCOM_SENDS_ON_LANDED times,
//         //once every SATCOM_LANDED_TIME_INTERVAL
//         landedSatcomSentCount++;
//         SatComSendGPS(&timestamp, GPS_data);
//         satcomMsgOldTime = millis();
//     }
// }

/**
  * @brief  Helper function for LED blinking
  * @return None
  */
inline void blinkStatusLED() {
    static unsigned long init_st_old_time = 0;
    static const uint16_t init_st_time_interval = 500;
    static bool init_st_indicator = false;

    if (s_statusOfInit == Status::NONCRITICAL_FAILURE && millis() - init_st_old_time > init_st_time_interval) {
        init_st_old_time = millis();
        init_st_indicator = !init_st_indicator;

        if (init_st_indicator)
            digitalWrite(LED_BUILTIN, HIGH);
        else
            digitalWrite(LED_BUILTIN, LOW);
    }
}
