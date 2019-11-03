/*Includes------------------------------------------------------------*/
#include "sensors/temperature.h"

SensorStatus Temperature::initSensor() {
    temp_sensor = new TMP102(TEMP_SENSOR_ADDRESS);

    /*init temp sensor*/
    #ifdef TESTING
        SerialUSB.println("Initializing temperature sensor");
    #endif
    temp_sensor->begin();

    return NOMINAL;
}

SensorStatus Temperature::readData(float* data) {
    #ifdef TESTING
        SerialUSB.println("Polling temperature sensor");
    #endif

    temp = temp_sensor->readTempC();
    data[0] = temp;

    return NOMINAL;
}

uint8_t Temperature::dataLength() {
    return TEMPERATURE_DATA_ARRAY_SIZE;
}