/*Includes------------------------------------------------------------*/
#include "sensors/barometer.h"

SensorStatus Barometer::initSensor() {
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
        return CRITICAL_FAILURE;
    }
    #endif

    return NOMINAL;
}

SensorStatus Barometer::readData(float* data) {
    #ifdef TESTING
        SerialUSB.println("Polling barometer");
        bool bar_flag = barometer.readSensor();
        if(!bar_flag) {
            SerialUSB.println("BAROMETER FAILED READING");
            return NOMINAL;
        }
    #else
        barometer->readSensor();
    #endif

    data[0] = barometer->pressure();
    data[1] = barometer->temperature();

    return NOMINAL;
}