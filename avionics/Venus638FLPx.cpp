/*GPS Venus638FLPx Driver Source*/

/*Includes------------------------------------------------------------*/
#include "Venus638FLPx.h"
#include "sensors.h"

#include <Arduino.h>

/*Variables------------------------------------------------------------*/

/*Functions------------------------------------------------------------*/

bool getGPSData(char* sentence)
{
    bool status = false;
    int i = 0;
    char field[20];
    elapsedMillis timeout;

    if (SerialGPS.available()) {
        while (SerialGPS.available() && (status == false) && (timeout < GPS_TIMEOUT)) {
            char ch = SerialGPS.read();
            if (ch != '\n' && i < SENTENCE_SIZE) {
                sentence[i] = ch;
                i++;
            } else {
                sentence[i] = '\0';
                i = 0;
                getGPSField(sentence, field, 0);
                if (strcmp(field, "$GPGGA") == 0) {
                    status = true;
                }
            }
        }
    }

    return status;
}

void getGPSField(char* sentence, char* buffer, int index)
{
    int sentencePos = 0;
    int fieldPos = 0;
    int commaCount = 0;

    while (sentencePos < SENTENCE_SIZE) {
        if (sentence[sentencePos] == ',') {
            commaCount++;
            sentencePos++;
        }
        if (commaCount == index) {
            buffer[fieldPos] = sentence[sentencePos];
            fieldPos++;
        }
        sentencePos++;
    }
    buffer[fieldPos] = '\0';
}
