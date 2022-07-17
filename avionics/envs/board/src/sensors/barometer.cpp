/*Includes------------------------------------------------------------*/
#include "options.h"
#include "sensors/barometer.h"

Barometer::Barometer(float *const buf) : SensorBase(buf) {
    /*init barometer*/
#ifdef TESTING
    SerialUSB.println("Initializing barometer");
    #ifdef STAGE2
        SerialUSB.println("for stage 2");
    #endif
    // replaced for tantalus lite version
    // if (!barometer.initializeMS_5803(true)) { // because one is verbose
    //     return CRITICAL_FAILURE;
    // }
    if (!bmp.begin()) {
        SerialUSB.println("Sensor not connected!");
        while (1);
    }
#else
    // replaced for tantalus lite version
    // if (!barometer.initializeMS_5803(false)) {
    //     status = SensorStatus::FAILURE;
    // }
    if (!bmp.begin()) {
        Serial.println("Sensor not connected!");
        status = SensorStatus::FAILURE;
    }
#endif

    status = SensorStatus::NOMINAL;
}

void Barometer::readData() {
#ifdef TESTING
    SerialUSB.println("Polling barometer");
    // removed for tantalus lite version
    // bool bar_flag = barometer.readSensor();
    // if (!bar_flag) {
    //     SerialUSB.println("BAROMETER FAILED READING");
    //     return NOMINAL;
    // }
#else
    // replaced for tantalus lite version
    // barometer.readSensor();
#endif
    // replaced for tantalus lite version
    // barometer.pressure() returns mbar; we want Pa
    // data_[0] = barometer.pressure() * 100;
    // data_[1] = barometer.temperature();

    // TODO: Create abstract barometer so we can use any barometer library here
#ifdef STAGE2
    bmp.read();
    data_[0] = bmp.getPressure() * 100; // MS5611 gives mbar, we want Pa
    data_[1] = bmp.getTemperature(); 
    #ifdef TESTING
        SerialUSB.print("pressure: ");
        SerialUSB.print(bmp.getPressure() * 100);
        SerialUSB.print("Pa, temp: ");
        SerialUSB.print(bmp.getTemperature());
        SerialUSB.println("degC");
    #endif
#else
    data_[0] = bmp.readPressure(); // BMP085 gives Pa
    data_[1] = bmp.readTemperature(); 
    #ifdef TESTING
        SerialUSB.print("pressure: ");
        SerialUSB.print(bmp.readPressure());
        SerialUSB.print("Pa, temp: ");
        SerialUSB.print(bmp.readTemperature());
        SerialUSB.println("degC");
    #endif
#endif
}
