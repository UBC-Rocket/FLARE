/*Includes------------------------------------------------------------*/
#include "sensors/barometer.h"
#include "options.h"
#include "log.hpp"

Barometer::Barometer(float *const buf) : SensorBase(buf) , barometer(&Wire) {
    /*init barometer*/
    barometer.setI2Caddr(0b1110111);

#ifdef TESTING
    LOG_DEBUG("Initializing barometer");
    if (barometer.connect() > 0) {
        LOG_DEBUG("Error connecting to barometer");
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
    LOG_DEBUG("Polling barometer");
    LOG_DEBUG(("Pressure [Pa]: " + std::to_string(data_[0])).c_str());
    LOG_DEBUG(("Temperature [0.01 C]: " + std::to_string(data_[1])).c_str());
#endif
}
