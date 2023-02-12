/*Includes------------------------------------------------------------*/
#include "sensors/barometer.h"

Barometer::Barometer(float *const buf) : SensorBase(buf) , barometer(&Wire) {
    /*init barometer*/
    barometer.setI2Caddr(0b1110111);

#ifdef TESTING
    SerialUSB.println("Initializing barometer");
    if (barometer.connect() > 0) {
        SerialUSB.println("Error connecting to barometer");
        return CRITICAL_FAILURE;
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
    SerialUSB.println("Polling barometer");
    SerialUSB.print("Temperature [0.01 C]: ");
    SerialUSB.println(data_[0]);
    SerialUSB.print("Pressure [Pa]: ");
    SerialUSB.println(data_[1]);
#endif
}
