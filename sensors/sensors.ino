/*Main Arduino Sketch*/

/*Includes------------------------------------------------------------*/
#include "sensors.h"

#include <Arduino.h>
#include <HardwareSerial.h>
#include <i2c_t3.h>
#include <SD.h>

/*Constants------------------------------------------------------------*/

/*Variables------------------------------------------------------------*/

/*Functions------------------------------------------------------------*/
void setup()
{
    bool status = true;

    /*init serial comms*/
    Serial.begin(9600);
    while (!Serial) {}
    #ifdef TESTING
    Serial.println("Initializing...");
    #endif

    /*init I2C bus*/
    Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_EXT, I2C_RATE_400); //400kHz
    Wire.setDefaultTimeout(100000); //100ms

    /*init sensors*/
    status = initSensors();

    /*if something went wrong spin infinitely, otherwise indicate completion*/
    if (!status) {
        Serial.println("Initialization failed! >:-{");
        while (1) {}
    } else {
        pinMode(LED_BUILTIN,OUTPUT);
        digitalWrite(LED_BUILTIN,HIGH);
        Serial.println("Initialization complete! :D");
    }
}

void loop()
{
    float acc_data[ACC_DATA_ARRAY_SIZE], bar_data[BAR_DATA_ARRAY_SIZE],
        temp_sensor_data, IMU_data[IMU_DATA_ARRAY_SIZE];
    char GPS_data[GPS_DATA_ARRAY_SIZE][GPS_SENTENCE_LENGTH];

    char temp;
    while (Serial2.available()) {
        temp = Serial2.read();
        if (temp == '\0') {
            Serial.println();
        } else {
            Serial.print(temp);
        }
    }
    pollSensors(acc_data, bar_data, &temp_sensor_data, IMU_data, GPS_data);
    //run algorithm
    logData(acc_data, bar_data, &temp_sensor_data, IMU_data, GPS_data);
    Serial2.print(bar_data[0]);

    #ifdef TESTING
    delay(1000);
    #endif
}
