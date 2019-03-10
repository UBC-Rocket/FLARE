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
On Mac: ‎⁨Contents⁩/⁨Java⁩/⁨hardware⁩/⁨teensy⁩/⁨avr⁩/⁨cores⁩/⁨teensy3⁩/serial1.c
On Windows: [user_drive]\Program Files (x86)\Arduino\hardware\teensy\avr\cores\teensy3\serial1.c

On line 43 increase SERIAL1_RX_BUFFER_SIZE from 64 to 128.
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
#include "sensors.h"
#include "statemachine.h"
#include "calculations.h"
#include "commands.h"
#include "gpio.h"
#include "radio.h"
#include "groundaltitude.h"
#include "satcom.h"

#include <Arduino.h>
#include <HardwareSerial.h>
#include <i2c_t3.h>
#include <SD.h>
#include <string.h>

/*Variables------------------------------------------------------------*/
File radiolog;
static InitStatus s_statusOfInit;
static float pressure_set[PRESSURE_AVG_SET_SIZE]; //set of pressure values for a floating average
static float baseline_pressure;
static float ground_alt_arr[GROUND_ALT_SIZE]; //values for the baseline pressure calculation

/*Functions------------------------------------------------------------*/
/**
  * @brief  The Arduino setup function
  * @param  None
  * @return None
  */
void setup()
{
    int i;
    #ifdef BODY
        initPins();
    #endif

    /*init serial comms*/
    #ifdef TESTING
    SerialUSB.begin(9600);
    while (!SerialUSB) {} //TODO add print in while to see what happens
    SerialUSB.println("Initializing...");
    #endif

    /*init I2C bus*/
    Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_EXT, I2C_RATE_400); //400kHz
    Wire.setDefaultTimeout(100000); //100ms

    /*init sensors*/
    initSensors(&s_statusOfInit);

    /*if something went wrong spin infinitely, otherwise indicate completion*/
    if (s_statusOfInit.overview == CRITICAL_FAILURE) {
        #ifdef TESTING
        SerialUSB.println("Critical failure! >:-{");
        #endif
    }
    else if (s_statusOfInit.overview == NONCRITICAL_FAILURE){
        #ifdef TESTING
        SerialUSB.println("Noncritical failure! :(");
        #endif

        pinMode(LED_BUILTIN, OUTPUT);
    }
    else {
        pinMode(LED_BUILTIN,OUTPUT);
        digitalWrite(LED_BUILTIN,HIGH);
        #ifdef TESTING
        SerialUSB.println("Initialization complete! :D");
        #endif

    }

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
    static unsigned long timestamp;
    static unsigned long old_time = 0; //ms
    static unsigned long new_time = 0; //ms
    unsigned long delta_time;
    static uint16_t time_interval = NOMINAL_POLLING_TIME_INTERVAL; //ms


    static unsigned long tier_one_old_time = 0;
    static unsigned long tier_two_old_time = 0;
    static unsigned long tier_three_old_time = 0;


    static unsigned long init_status_old_time = 0;
    static unsigned long init_status_new_time = 0;
    static const uint16_t init_status_time_interval = 500;
    static uint16_t init_status_indicator = 0;

    static float battery_voltage, acc_data[ACC_DATA_ARRAY_SIZE], bar_data[BAR_DATA_ARRAY_SIZE],
        temp_sensor_data, IMU_data[IMU_DATA_ARRAY_SIZE], GPS_data[GPS_DATA_ARRAY_SIZE];

    static float prev_altitude, altitude, delta_altitude, prev_delta_altitude, ground_altitude;

    static FlightStates state = STANDBY;


    static uint16_t tier_one_interval = 400;
    static uint16_t tier_two_interval = 2000;
    static uint16_t tier_three_interval = 20000;


    char command[RADIO_DATA_ARRAY_SIZE];
    char recognitionRadio[RADIO_DATA_ARRAY_SIZE];
    char goodResponse[] = {'G','x','x','x','x'};
    const char badResponse[] = {'B','B','B','B','B'};

    #ifdef NOSECONE
        char satComCommandArray[SAT_COM_DATA_ARRAY_SIZE];
        static bool mainDeploySatcomSent = false;
        static int landedSatcomSentCount = 0;
        static uint16_t satcomMsgOldTime = millis();
    #endif

    if(s_statusOfInit.overview == CRITICAL_FAILURE)
        state = WINTER_CONTINGENCY; //makes sure that even if it does somehow get accidentally changed, it gets reverted

    if (SerialRadio.available() >= 5) {

        #ifdef TESTING
        SerialUSB.print("Received Message: ");
        #endif

        for(int i = 0; i< RADIO_DATA_ARRAY_SIZE; i++){
            command[i] = SerialRadio.read();
        }

        bool correctCommand = check(command);

        if(correctCommand){
            for(int i =1; i<5; i++)
            {
                goodResponse[i] = command[0];
            }

            #ifdef TESTING
            SerialUSB.print("Good command: ");
            SerialUSB.println(command);
            #endif

            doCommand(command[0], &state, &s_statusOfInit);
            sendRadioResponse(goodResponse);
        }
        else{
            #ifdef TESTING
            SerialUSB.print("Bad command: ");
            SerialUSB.println(command);
            #endif

            sendRadioResponse(badResponse);
        }
    }

    #ifdef NOSECONE
        //SatCom receive check
        if (SatComReceive(satComCommandArray))
        {
            #ifdef TESTING
                for(int q = 0; q < SAT_COM_DATA_ARRAY_SIZE; q++){
                    SerialUSB.write(satComCommandArray[q]);
                }
                SerialUSB.println();
            #endif

            if(check(satComCommandArray)) // this check array will move and be renamed
            {
                #ifdef TESTING
                SerialUSB.print("Good Command: ");
                SerialUSB.write(satComCommandArray[0]);
                SerialUSB.println();
                #endif

                doCommand(satComCommandArray[0], &state, &s_statusOfInit);
                // send sat com command back through sat com  ???
            }
            else
            {
                #ifdef TESTING
                SerialUSB.print("Bad Command: ");
                for (int b = 0; b < 5; b++){
                SerialUSB.write(satComCommandArray[b]);
                SerialUSB.println();
                }
                #endif

                // send sat com error back through sat com ????
            }
        }

        /* send radio data */
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

    if(state == STANDBY)
        time_interval = STANDBY_POLLING_TIME_INTERVAL;
    else if (state == LANDED)
        time_interval = LANDED_POLLING_TIME_INTERVAL;
    else
        time_interval = NOMINAL_POLLING_TIME_INTERVAL;

    new_time = millis();
    if ((new_time - old_time) >= time_interval) {
        delta_time = new_time - old_time;
        old_time = new_time;

        pollSensors(&timestamp, &battery_voltage, acc_data, bar_data, &temp_sensor_data, IMU_data, GPS_data);
        calculateValues(acc_data, bar_data, &prev_altitude, &altitude, &delta_altitude, &prev_delta_altitude, &baseline_pressure, &delta_time, pressure_set);
        stateMachine(&altitude, &delta_altitude, &prev_altitude, bar_data, &baseline_pressure, &ground_altitude, ground_alt_arr, &state);
        logData(&timestamp, &battery_voltage, acc_data, bar_data, &temp_sensor_data, IMU_data, GPS_data, state, altitude, baseline_pressure);
    }

    if((new_time - tier_one_old_time) >= tier_one_interval) {
        // sendTierOne(&timestamp, GPS_data, bar_data, state, altitude);
        #ifdef BODY
            bodyTierOne(bar_data, state, altitude, &timestamp);
        #endif
        #ifdef NOSECONE
            noseconeTierOne(&timestamp, GPS_data);
        #endif
        tier_one_old_time = new_time;
    }

    if ( (new_time - tier_two_old_time) >= tier_two_interval ){
        // sendTierTwo(acc_data, bar_data, &temp_sensor_data, IMU_data);
        #ifdef NOSECONE
            noseconeTierTwo(bar_data, acc_data, &temp_sensor_data, IMU_data);
        #endif
        tier_two_old_time = new_time;
    }

    // Anything that would go in tier_three would probably only be sent once @ start and on radio request
    // if ( (new_time - tier_three_old_time) >= tier_three_interval ){
    //     sendTierThree(&battery_voltage, &ground_altitude);
    //     tier_three_old_time = new_time;
    // }

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

//checks if all indexes are equal for radio commands
bool check(char *radioCommand)
 {
    const char a0 = radioCommand[0];

    for (int i = 1; i < 5; i++)
    {
        if (radioCommand[i] != a0)
            return false;
    }
    return true;
}
