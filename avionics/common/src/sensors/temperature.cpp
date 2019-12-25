/*Includes------------------------------------------------------------*/
#include "sensors/temperature.h"

void Temperature::initSensor() {
    temp_sensor = new TMP102(TEMP_SENSOR_ADDRESS);

    /*init temp sensor*/
    #ifdef TESTING
        SerialUSB.println("Initializing temperature sensor");
    #endif
    temp_sensor->begin();

    status = SensorStatus::NOMINAL;
}

void Temperature::readData() {
    #ifdef TESTING
        SerialUSB.println("Polling temperature sensor");
    #endif

    temp = temp_sensor->readTempC();
    data[0] = temp;
}

uint8_t Temperature::dataLength() {
    return TEMPERATURE_DATA_ARRAY_SIZE;
}

float *Temperature::getData() {
    return data;
}