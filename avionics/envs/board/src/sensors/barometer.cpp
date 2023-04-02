/*Includes------------------------------------------------------------*/
#include "sensors/barometer.h"
#include "options.h"

Barometer::Barometer(float *const buf) : SensorBase(buf) , barometer(&Wire) {
    /*init barometer*/
    barometer.setI2Caddr(0b1110111);

#ifdef TESTING
    Serial.println("Initializing barometer");
    if (barometer.connect() > 0) {
        Serial.println("Error connecting to barometer");
        status = SensorStatus::FAILURE;
    }
#else
    if (barometer.connect() > 0) {
        status = SensorStatus::FAILURE;
    }
    
#endif

    status = SensorStatus::NOMINAL;
}

void Barometer::readData() {

    barometer.ReadProm();
    barometer.Readout();

    data_[0] = barometer.GetPres();
    data_[1] = barometer.GetTemp();

#ifdef TESTING
    Serial.println("Polling barometer");
    Serial.print("Temperature [0.01 C]: ");
    Serial.println(data_[0]);
    Serial.print("Pressure [Pa]: ");
    Serial.println(data_[1]);
#endif
}
