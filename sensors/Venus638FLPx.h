/*GPS Venus638FLPx Driver Header*/

#ifndef Venus638FLPx_h
#define Venus638FLPx_h

#include <Arduino.h> //for elapsedMillis
#include <HardwareSerial.h>

#define SENTENCE_SIZE 100

class Venus638FLPx {
    public:
        Venus638FLPx(HardwareSerial serial);
        void getGPS(bool* status);
        void getField(char* buffer, int index);

    private:
        HardwareSerial _serial;
        const int _sentenceSize = SENTENCE_SIZE;
        char _sentence[SENTENCE_SIZE];
};

#endif
