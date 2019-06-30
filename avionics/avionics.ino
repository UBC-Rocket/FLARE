/*Main Arduino Sketch*/
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

/*Includes------------------------------------------------------------*/
#include "buzzer.h"
#include "calculations.h"
#include "cameras.h"
#include "gpio.h"
#include "groundaltitude.h"
#include "options.h"
#include "radio.h"
#include "satcom.h"
#include "sensors.h"
#include "statemachine.h"
#include "xbee.h"

#include <Arduino.h>
#include <HardwareSerial.h>
#include <i2c_t3.h>
#include <SD.h>
#include <SPI.h>
#include <string.h>

/*Errors---------------------------------------------------------------*/
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

/*Variables------------------------------------------------------------*/
File radiolog;
static InitStatus s_statusOfInit;
static float pressure_set[PRESSURE_AVG_SET_SIZE]; //set of pressure values for a floating average
static float baseline_pressure;
static float ground_alt_arr[GROUND_ALT_SIZE]; //values for the baseline pressure calculation

static XBee s_radio = XBee();
static XBeeAddress64 s_groundAddress = XBeeAddress64(GROUND_STATION_ADDR_MSB, GROUND_STATION_ADDR_LSB);
static ZBTxRequest s_txPacket = ZBTxRequest();

/*Functions------------------------------------------------------------*/
/**
  * @brief  The Arduino setup function
  * @param  None
  * @return None
  */
void setup()
{
    int i;

    initPins();

    /* Setup all UART comms */
    // Serial comms to computer
    #ifdef TESTING
    SerialUSB.begin(9600);
    while (!SerialUSB) {}
    SerialUSB.println("Initializing...");
    #endif

    // Comms to radio serial port
    #ifdef TESTING
    SerialUSB.println("Initializing radio");
    #endif
    SerialRadio.begin(921600);
    while (!SerialRadio) {}
    s_radio.setSerial(SerialRadio);
    s_txPacket.setAddress64(s_groundAddress);

    // Comms to camera serial port
    SerialCamera.begin(CameraBaud);
    while (!SerialCamera) {}

    /*init I2C bus*/
    Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_EXT, I2C_RATE_400); //400kHz
    Wire.setDefaultTimeout(100000); //100ms

    /*init sensors and report status in many ways*/
    initSensors(&s_statusOfInit);
    radioStatus(&s_radio, &s_txPacket, &s_statusOfInit);

    /* init various arrays */
    baseline_pressure = barSensorInit(); /* for baseline pressure calculation */
    for (i = 0; i < PRESSURE_AVG_SET_SIZE; i++) // for moving average
    {
        pressure_set[i] = baseline_pressure;
    }

    for(i = 0; i < GROUND_ALT_SIZE; i++)
    {
        ground_alt_arr[i] = baseline_pressure;
    }

}

/**
  * @brief  The Arduino loop function
  * @param  None
  * @return None
  */
void loop()
{
    static uint32_t timestamp;
    static unsigned long old_time = 0; //ms
    static unsigned long new_time = 0; //ms
    unsigned long delta_time;
    static uint16_t time_interval = NOMINAL_POLLING_TIME_INTERVAL; //ms

    static unsigned long init_status_old_time = 0;
    static unsigned long init_status_new_time = 0;
    static const uint16_t init_status_time_interval = 500;
    static uint16_t init_status_indicator = 0;

    static float battery_voltage, acc_data[ACC_DATA_ARRAY_SIZE], bar_data[BAR_DATA_ARRAY_SIZE],
        temp_sensor_data, IMU_data[IMU_DATA_ARRAY_SIZE], GPS_data[GPS_DATA_ARRAY_SIZE], thermocouple_data;

    static float prev_altitude, altitude, delta_altitude, prev_delta_altitude, ground_altitude;

    static FlightStates state = STANDBY;

    static unsigned long radio_old_time = 0;
    static unsigned long radio_time_interval = 500; //milliseconds

    #ifdef NOSECONE
        static bool mainDeploySatcomSent = false;
        static int landedSatcomSentCount = 0;
        static uint16_t satcomMsgOldTime = millis();
    #endif


    if(s_statusOfInit.overview == CRITICAL_FAILURE)
        state = WINTER_CONTINGENCY; //makes sure that even if it does somehow get accidentally changed, it gets reverted

    // if radio communications are received
    resolveRadioRx(&s_radio, &s_txPacket, GPS_data, &state, &s_statusOfInit);

    #ifdef NOSECONE
     /* send satcom data */
    if(state == FINAL_DESCENT && !mainDeploySatcomSent)
    {
        mainDeploySatcomSent = true;
        SatComSendGPS(&timestamp, GPS_data);
    }
    else if(state == LANDED && landedSatcomSentCount < NUM_SATCOM_SENDS_ON_LANDED && millis() - satcomMsgOldTime >= SATCOM_LANDED_TIME_INTERVAL)
    { //sends Satcom total of NUM_SATCOM_SENDS_ON_LANDED times, once every SATCOM_LANDED_TIME_INTERVAL
        landedSatcomSentCount++;
        SatComSendGPS(&timestamp, GPS_data);
        satcomMsgOldTime = millis();
    }

    #endif //def NOSECONE

    if (state == LANDED)
        time_interval = LANDED_POLLING_TIME_INTERVAL;
    else
        time_interval = NOMINAL_POLLING_TIME_INTERVAL;


    new_time = millis();
    if ((new_time - old_time) >= time_interval) {
        delta_time = new_time - old_time;
        old_time = new_time;

        pollSensors(&timestamp, &battery_voltage, acc_data, bar_data, &temp_sensor_data, IMU_data, GPS_data, &thermocouple_data);
        calculateValues(acc_data, bar_data, &prev_altitude, &altitude, &delta_altitude, &prev_delta_altitude, &baseline_pressure, &delta_time, pressure_set);
        stateMachine(&altitude, &delta_altitude, &prev_altitude, bar_data, &baseline_pressure, &ground_altitude, ground_alt_arr, &state);
        logData(&timestamp, &battery_voltage, acc_data, bar_data, &temp_sensor_data, IMU_data, GPS_data, state, altitude, baseline_pressure, thermocouple_data);
    }

    if((new_time - radio_old_time) >= radio_time_interval) {
        #ifdef BODY
            sendRadioBody(&s_radio, &s_txPacket, bar_data, state, &altitude, &timestamp);
        #endif  // def BODY
        #ifdef NOSECONE
            sendRadioNosecone(&s_radio, &s_txPacket, GPS_data, bar_data, acc_data, &temp_sensor_data, IMU_data);
        #endif  //def NOSECONE
        radio_old_time = new_time;
    }

    if (s_statusOfInit.overview == NONCRITICAL_FAILURE)
    {
        init_status_new_time = millis();
        if ( (init_status_new_time - init_status_old_time) > init_status_time_interval ){
            init_status_old_time = init_status_new_time;
            init_status_indicator++;

            if(init_status_indicator % 2 == 1)
                digitalWrite(LED_BUILTIN, HIGH);
            else
                digitalWrite(LED_BUILTIN, LOW);
        }
    }

    #ifdef TESTING
    delay(1000);
    #endif
}
