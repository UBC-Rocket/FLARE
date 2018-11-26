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
	bool dolla$ = false;
    int i = 0;
	char c = 'a';
    elapsedMillis timeout;

    if (SerialGPS.available()) {
		//gps sentence is 70 characters long, buffer can only hold 64 seemingly, only need up to ~40 for altitude
		while (SerialGPS.available() && timeout < GPS_TIMEOUT && i < SENTENCE_SIZE) {
			c = SerialGPS.read();

			//uncomment for testing
			// Serial.print("the charcter is (drumrole): ");
			// Serial.println(c);
			// Serial.print("the position is: ");
			// Serial.println(i);

			//check to see if the first thing in the buffer is the start of the gps sentence
			if (c == '$') {
				//can move on to reading the sentence
				dolla$ = true;
				sentence[i] = c;
				i++;
			}
			//continue reading the sentence once a $ is read
			if (dolla$) {
				sentence[i] = c;
				i++;
			}
		}
    }
	//if all 60 characters are read we have a succesfull gps read
	//The G in position 4 is a unique character to $GPGGA
	if (i == SENTENCE_SIZE && sentence[4]=='G') {
		status = true;
	}

    return status;
}

void getGPSField(char* sentence, char* buffer, int index)
{
    int sentencePos = 0;
    int fieldPos = 0;
    int commaCount = 0;
	//only 60 characters in the sentence
    while (sentencePos <= SENTENCE_SIZE) {
        if (sentence[sentencePos] == ',') {
            commaCount++;
            sentencePos++;
        }
        if (commaCount == index) {
            buffer[fieldPos] = sentence[sentencePos];
            fieldPos++;
            //make sure it doesn't overflow the buffer
            if (fieldPos >= GPS_FIELD_LENGTH) {
            	break;
            }
        }
        sentencePos++;
    }
}
