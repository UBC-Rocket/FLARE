/*Includes------------------------------------------------------------*/
#include "MAX31855k.h"
#include "sensors/thermocouple.h"
#include <cmath>

/*Variables------------------------------------------------------------*/
#ifdef TESTING
// Self powered with debug output
MAX31855k probe(THERMO_SELECT_PIN, NONE, NONE, true);
#else
// Self powered with no debug messages
MAX31855k probe(THERMO_SELECT_PIN);
#endif // TESTING

void Thermocouple::initSensor() {
#ifdef TESTING
    SerialUSB.println("Initializing thermocouple");
#endif

    float thermo_temp = probe.readCJT();
    if (!std::isnan(thermo_temp)) {
#ifdef TESTING
        SerialUSB.print("Cold Junction Temperature is [C]: ");
        SerialUSB.println(thermo_temp);
        SerialUSB.println("Thermocouple initialized");
#endif
    }
    else {
        status = SensorStatus::FAILURE;
    }

    status = SensorStatus::NOMINAL;
}

void Thermocouple::readData() {
#ifdef TESTING
    SerialUSB.println("Polling Thermocouple");
#endif // TESTING
    data[0] = probe.readTempC();
#ifdef TESTING
    if (!std::isnan(*thermocouple_data)) {
        SerialUSB.print("Temp[C]=");
        SerialUSB.println(*thermocouple_data);
    }
    else {
        SerialUSB.println("Thermocouple ERROR");
    }
#endif // TESTING

    status = SensorStatus::NOMINAL;
}

uint8_t Thermocouple::dataLength() {
    return THERMOCOUPLE_DATA_ARRAY_SIZE;
}

float *Thermocouple::getData() {
    return data;
}
