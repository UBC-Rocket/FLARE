/*GPS Venus638FLPx Driver Source*/

#include "Venus638FLPx.h"

Venus638FLPx::Venus638FLPx(HardwareSerial serial){
    _serial = serial;
}

void Venus638FLPx::getGPS(bool* status)
{
    int i = 0;
    char field[20];
    elapsedMillis timeout;

    while (timeout < 100) {
        if (_serial.available()) {
            char ch = _serial.read();
            if (ch != '\n' && i < _sentenceSize) {
                _sentence[i] = ch;
                i++;
            } else {
                _sentence[i] = '\0';
                i = 0;
                getField(field, 0);
                if (strcmp(field, "$GPGGA") == 0) {
                    *status = true;
                    break;
                }
            }
        }
    }
}

void Venus638FLPx::getField(char* buffer, int index)
{
    int sentencePos = 0;
    int fieldPos = 0;
    int commaCount = 0;

    while (sentencePos < _sentenceSize) {
        if (_sentence[sentencePos] == ',') {
            commaCount++;
            sentencePos++;
        }
        if (commaCount == index) {
            buffer[fieldPos] = _sentence[sentencePos];
            fieldPos++;
        }
        sentencePos++;
    }
    buffer[fieldPos] = '\0';
}
