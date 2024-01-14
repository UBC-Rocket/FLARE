/*Includes------------------------------------------------------------*/
#include "sensors/barometer.h"
#include "options.h"

Barometer::Barometer(float *const buf) : SensorBase(buf) , barometer(&Wire) , fakePressure(101300), negative(-1) {
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

    // barometer.ReadProm();
    // barometer.Readout();

    // data_[0] = barometer.GetPres();
    
    // // Re-read in case of overflow
    // while (data_[0] < 0 || data_[0] > 102000) {
    //     barometer.ReadProm();
    //     barometer.Readout();
    //     data_[0] = barometer.GetPres();
    // }
    fakePressure += 2000 * negative;
    data_[0] = fakePressure;
    if(fakePressure < 60000){
        negative = 1;
    }else if(fakePressure > 101400){
        negative = 0;
    }
    
    data_[1] = barometer.GetTemp();

#ifdef TESTING
    Serial.println("Polling barometer");
    Serial.print("Pressure [Pa]: ");
    Serial.println(data_[0]);
    Serial.print("Temperature [0.01 C]: ");
    Serial.println(data_[1]);
#endif
}
