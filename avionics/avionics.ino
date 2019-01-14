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
    static bool loop_except_that_its_setup = false;

    unsigned long timestamp;
    static unsigned long old_time = 0; //ms
    static unsigned long new_time = 0; //ms
    unsigned long delta_time;
    static uint16_t time_interval = 50; //ms
    float acc_data[ACC_DATA_ARRAY_SIZE], bar_data[BAR_DATA_ARRAY_SIZE],
        temp_sensor_data, IMU_data[IMU_DATA_ARRAY_SIZE], GPS_data[GPS_DATA_ARRAY_SIZE];
    static float abs_accel, prev_altitude, altitude, delta_altitude, prev_delta_altitude, ground_altitude, average_pressure;
    static FlightStates state = ARMED;
    static float pressure_set[PRESSURE_AVG_SET_SIZE]; //set of pressure values for a floating average
    static float ground_alt_arr[GROUND_ALT_SIZE];
    static float baseline_pressure = barSensorInit();


    if(!loop_except_that_its_setup) //loop setup
    {
        // static float baseline_pressure = barSensorInit();
        //static float baseline_pressure = groundAlt_init(&barometer_data_init);  // IF YOU CAN'T DO THIS USE GLOBAL VAR

        //initialize arrays
        int i;
        for (i = 0; i < PRESSURE_AVG_SET_SIZE; i++)
        {
            pressure_set[i] = baseline_pressure;
        }
        for(i = 0; i < GROUND_ALT_SIZE; i++)
        {
            ground_alt_arr[i] = baseline_pressure;
        }
        loop_except_that_its_setup = true;
    }



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
        addToPressureSet(pressure_set, bar_data[0]);
        average_pressure = calculatePressureAverage(pressure_set);
        calculateValues(acc_data, bar_data, &prev_altitude, &altitude, &delta_altitude, &prev_delta_altitude, &baseline_pressure, &delta_time, &average_pressure);
        stateMachine(&altitude, &delta_altitude, &prev_altitude, bar_data, &baseline_pressure, &ground_altitude, ground_alt_arr, &state);
        logData(&timestamp, acc_data, bar_data, &temp_sensor_data, IMU_data, GPS_data, state, altitude, baseline_pressure);
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
