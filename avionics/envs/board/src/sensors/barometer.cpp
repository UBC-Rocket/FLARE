/*Includes------------------------------------------------------------*/
#include "sensors/barometer.h"

Barometer::Barometer(float *const buf) : SensorBase(buf) {
    /*init barometer*/
#ifdef TESTING
    SerialUSB.println("Initializing barometer");
    if (!barometer.initializeMS_5803(true)) { // because one is verbose
        return CRITICAL_FAILURE;
    }
#else
    if (!barometer.initializeMS_5803(false)) {
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
    barometer.readSensor();
#endif

    // barometer.pressure() returns mbar; we want Pa
    data_[0] = barometer.pressure() * 100;
    data_[1] = barometer.temperature();
}
