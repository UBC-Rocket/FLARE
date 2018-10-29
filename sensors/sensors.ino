/*Main Arduino Sketch*/

/*Includes------------------------------------------------------------*/
#include "sensors.h"

#include <Arduino.h>
#include <HardwareSerial.h>
#include <i2c_t3.h>
#include <SD.h>

/*Constants------------------------------------------------------------*/

/*Variables------------------------------------------------------------*/
File radiolog;

/*Functions------------------------------------------------------------*/
void setup()
{
    bool status = true;

    /*init serial comms*/
    #ifdef TESTING
    SerialUSB.begin(9600);
    while (!SerialUSB) {}
    SerialUSB.println("Initializing...");
    #endif

    /*init I2C bus*/
    Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_EXT, I2C_RATE_400); //400kHz
    Wire.setDefaultTimeout(100000); //100ms

    /*init sensors*/
    status = initSensors();

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

void loop()
{
    float acc_data[ACC_DATA_ARRAY_SIZE], bar_data[BAR_DATA_ARRAY_SIZE],
        temp_sensor_data, IMU_data[IMU_DATA_ARRAY_SIZE];
    char GPS_data[GPS_DATA_ARRAY_SIZE][GPS_SENTENCE_LENGTH];

    radiolog.print("Received Message: ");
    #ifdef TESTING
    SerialUSB.print("Received Message: ");
    #endif
    while (SerialRadio.available()) {
        char temp = SerialRadio.read();
        if (temp == '\n') {
            radiolog.println();
            #ifdef TESTING
            SerialUSB.println();
            #endif
        } else {
            radiolog.print(temp);
            #ifdef TESTING
            SerialUSB.print(temp);
            #endif
        }
    }

    pollSensors(acc_data, bar_data, &temp_sensor_data, IMU_data, GPS_data);
    //algorithm
    logData(acc_data, bar_data, &temp_sensor_data, IMU_data, GPS_data);

    SerialRadio.print(bar_data[0]);
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
