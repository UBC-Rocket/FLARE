/*Main Arduino Sketch*/

/*Includes------------------------------------------------------------*/
#include "sensors.h"
#include "statemachine.h"
#include "calculations.h"

#include <Arduino.h>
#include <HardwareSerial.h>
#include <i2c_t3.h>
#include <SD.h>

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
        #endif
        while (1) {}
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
    unsigned long delta_time;
    static uint16_t time_interval = 5000; //ms
    float acc_data[ACC_DATA_ARRAY_SIZE], bar_data[BAR_DATA_ARRAY_SIZE],
        temp_sensor_data, IMU_data[IMU_DATA_ARRAY_SIZE];
    char GPS_data[GPS_DATA_ARRAY_SIZE][GPS_FIELD_LENGTH];
    static float abs_accel, prev_altitude, altitude, delta_altitude, prev_delta_altitude, ground_altitude;
    static FlightStates state = STANDBY;

    if (SerialRadio.available()) {
        radiolog.print("Received Message: ");
        #ifdef TESTING
        SerialUSB.print("Received Message: ");
        #endif
        while (SerialRadio.available()) {
            char command = SerialRadio.read();
            radiolog.print(command);
            #ifdef TESTING
            SerialUSB.print(command);
            #endif
        }
    }

    new_time = millis();
    if ((new_time - old_time) > time_interval) {
        delta_time = new_time - old_time;
        old_time = new_time;
        pollSensors(&timestamp, acc_data, bar_data, &temp_sensor_data, IMU_data, GPS_data);
        calculateValues(acc_data, bar_data, &prev_altitude, &altitude, &delta_altitude, &prev_delta_altitude, &baseline_pressure, &delta_time);
        stateMachine(&altitude, &delta_altitude, &prev_altitude, bar_data, &baseline_pressure, &ground_altitude, &state);
        logData(&timestamp, acc_data, bar_data, &temp_sensor_data, IMU_data, GPS_data);
    }

    SerialRadio.println(bar_data[0]);
    radiolog.print("Sent Message: ");
    radiolog.println(bar_data[0]);
    #ifdef TESTING
    SerialUSB.print("Sent Message: ");
    SerialUSB.println(bar_data[0]);
    #endif

    #ifdef TESTING
    delay(1000);
    #endif
}
