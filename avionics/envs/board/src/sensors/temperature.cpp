/*Includes------------------------------------------------------------*/
#include "sensors/temperature.h"

Temperature::Temperature(float *const data)
    : SensorBase(data), temp_sensor(TEMP_SENSOR_ADDRESS) {

/*init temp sensor*/
#ifdef TESTING
    SerialUSB.println("Initializing temperature sensor");
#endif
    temp_sensor.begin();
    status = SensorStatus::NOMINAL;
}

void Temperature::readData() {
#ifdef TESTING
    SerialUSB.println("Polling temperature sensor");
#endif
    data_[0] = temp_sensor.readTempC();
}
