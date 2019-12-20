/*Includes------------------------------------------------------------*/
#include "sensors/barometer.h"

void Barometer::initSensor() {
    barometer = new MS_5803(1024);

/*init barometer*/
#ifdef TESTING
    SerialUSB.println("Initializing barometer");
#endif
#ifdef TESTING
    if (!barometer.initializeMS_5803(true)) { //because one is verbose
        return CRITICAL_FAILURE;
    }
#else
    if (!barometer->initializeMS_5803(false)) {
        status = SensorStatus::FAILURE;
    }
#endif

    status = SensorStatus::NOMINAL;
}

void Barometer::readData() {
#ifdef TESTING
    SerialUSB.println("Polling barometer");
    bool bar_flag = barometer.readSensor();
    if (!bar_flag) {
        SerialUSB.println("BAROMETER FAILED READING");
        return NOMINAL;
    }
#else
    barometer->readSensor();
#endif

    data[0] = barometer->pressure();
    data[1] = barometer->temperature();
}

uint8_t Barometer::dataLength() {
    return BAROMETER_DATA_ARRAY_SIZE;
}

float *Barometer::getData() {
    return data;
}