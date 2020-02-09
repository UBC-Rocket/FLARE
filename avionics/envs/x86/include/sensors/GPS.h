#ifndef SENSORS__GPS_H_C87D138269654043A56EFCE31F6A40C5
#define SENSORS__GPS_H_C87D138269654043A56EFCE31F6A40C5


#include "sensors-interface.h"

/*Variables------------------------------------------------------------*/

class GPS : public ISensor {
private:
    static constexpr int GPS_DATA_ARRAY_SIZE = 3;
public:
    GPS(Hal::NativeSerial &seri){}
    void initSensor(){}
    void readData(){}
    uint8_t dataLength(){ return GPS_DATA_ARRAY_SIZE; }
    float *getData(){ return data; }
    SensorStatus getStatus(){ return SensorStatus::NOMINAL; }

private:
    float data[GPS_DATA_ARRAY_SIZE] = {49, -123, 0};
};

#endif
