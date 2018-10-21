#ifndef Venus638FLPx_h
#define Venus638FLPx_h

#include <HardwareSerial.h>

class Venus638FLPx {
    public:
        Venus638FLPx(HardwareSerial serial);
        void getGPS();
        void getField(char* buffer, int index);

    private:
        HardwareSerial _serial;
        const int _sentenceSize = 80;
        char _sentence[80];
};

#endif
