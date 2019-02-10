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
path in the Arduino contents folder: ‎⁨Contents⁩/⁨Java⁩/⁨hardware⁩/⁨teensy⁩/⁨avr⁩/⁨cores⁩/⁨teensy3⁩/serial1.c.
On line 43 increase SERIAL1_RX_BUFFER_SIZE from 64 to 128.
THIS MUST BE DONE ON THE COMPUTER USED TO COMPILE THE CODE!!!

VERY IMPORTANT PLEASE READ ME! VERY IMPORTANT PLEASE READ ME! VERY IMPORTANT PLEASE READ ME!
*/

/*Includes------------------------------------------------------------*/
#include "sensors.h"
#include "statemachine.h"
#include "calculations.h"
#include "commands.h"

#include <Arduino.h>
#include <HardwareSerial.h>
#include <i2c_t3.h>
#include <SD.h>
#include <string.h>

/*Variables------------------------------------------------------------*/
File radiolog;

/*Functions------------------------------------------------------------*/
/**
  * @brief  The Arduino setup function
  * @param  None
  * @return None
  */
void setup()
{
    bool status = true;

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
    status = initSensors();

    /*init interrupts*/
    //attachInterrupt(digitalPinToInterrupt(LAUNCH_INTERRUPT_PIN), launchInterrupt, CHANGE)

    /*if something went wrong spin infinitely, otherwise indicate completion*/
    if (!status) {
        #ifdef TESTING
        SerialUSB.println("Initialization failed! >:-{");
        #else
        while (1) {}
        #endif
    } else {
        pinMode(LED_BUILTIN,OUTPUT);
        digitalWrite(LED_BUILTIN,HIGH);
        #ifdef TESTING
        SerialUSB.println("Initialization complete! :D");
        #endif
    }
}

/**
  * @brief  The Arduino loop function
  * @param  None
  * @return None
  */
void loop()
{
    unsigned long timestamp;
    static float barometer_data_init = barSensorInit();
    static float baseline_pressure = groundAlt_init(&barometer_data_init);  // IF YOU CAN'T DO THIS USE GLOBAL VAR
    static unsigned long old_time = 0; //ms
    static unsigned long new_time = 0; //ms

    static unsigned long radio_old_time = 0;
    static unsigned long radio_new_time = 0;

    unsigned long delta_time;

    static uint16_t time_interval = 50; //ms

    float battery_voltage, acc_data[ACC_DATA_ARRAY_SIZE], bar_data[BAR_DATA_ARRAY_SIZE],
        temp_sensor_data, IMU_data[IMU_DATA_ARRAY_SIZE], GPS_data[GPS_DATA_ARRAY_SIZE];
    static float prev_altitude, altitude, delta_altitude, prev_delta_altitude, ground_altitude;
    static FlightStates state = ARMED;

    static uint16_t radio_time_interval = 100;
    char command[RADIO_DATA_ARRAY_SIZE];
    char recognitionRadio[RADIO_DATA_ARRAY_SIZE];
    char goodResponse[] = {'G','x','x','x','x'};
    const char badResponse[] = {'B','B','B','B','B'};

    if (SerialRadio.available()) {
        //radiolog.print("Received Message: ");
        #ifdef TESTING
        SerialUSB.print("Received Message: ");
        #endif
        while (SerialRadio.available()) {
            for(int i = 0; i< RADIO_DATA_ARRAY_SIZE; i++){
                command[i] = SerialRadio.read();
            }
            bool correctCommand = check(command);

            if(correctCommand){
               // radiolog.print(goodResponse);
               for(int i =1; i<5; i++)
               {
                   goodResponse[i] = command[0];
               }
                #ifdef TESTING
                SerialUSB.println(command);
                doCommand(command[0], &state);
                #endif

                sendRadioResponse(goodResponse);
                SerialUSB.println(goodResponse);
            }
            else{
                //radiolog.print(badResponse);
                SerialUSB.println(command);
                SerialUSB.println(goodResponse);
                sendRadioResponse(badResponse);
            }
        }
    }

    new_time = millis();
    if ((new_time - old_time) >= time_interval) {
        delta_time = new_time - old_time;
        old_time = new_time;
        pollSensors(&timestamp, &battery_voltage, acc_data, bar_data, &temp_sensor_data, IMU_data, GPS_data);
        calculateValues(acc_data, bar_data, &prev_altitude, &altitude, &delta_altitude, &prev_delta_altitude, &baseline_pressure, &delta_time);
        stateMachine(&altitude, &delta_altitude, &prev_altitude, bar_data, &baseline_pressure, &ground_altitude, &state);
        logData(&timestamp, &battery_voltage, acc_data, bar_data, &temp_sensor_data, IMU_data, GPS_data, state, altitude, baseline_pressure);
    }


    radio_new_time = millis();
    if ( (radio_new_time - radio_old_time) > radio_time_interval ){
        radio_old_time = radio_new_time;
        processRadioData(&timestamp, &battery_voltage, acc_data, bar_data, &temp_sensor_data, IMU_data, GPS_data, state, altitude);
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
