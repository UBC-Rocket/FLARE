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
    //char field[20];
    elapsedMillis timeout;
	char c = 'a';
	
    if (SerialGPS.available()) {
		//gps sentence is 70 characters long, buffer can only hold 64 seemingly, only need upto ~40 for altitude 
		while (SerialGPS.available() && timeout < GPS_TIMEOUT && i < 60) {
			c = SerialGPS.read();
			Serial.print("the charcter is (drumrole): ");
			Serial.println(c);
			Serial.print("the position is: ");
			Serial.println(i);
			//check to see if the first thing in the buffer is the start of the gps sentence
			if (c == '$') {
				sentence[i] = c; 
				//can move on in reading sentence
				dolla$ = true;
				i++;
				
			}
			//finishing reading the sentence once a $ is read
			if (dolla$) {
				sentence[i] = c;
				i++;
				
			}
		}
    }
	//if all 60 characters are read we have a succesfull gps read 
	if (i == 60) { 
		status = true;
		Serial.print("yo that shits true dawg");
	}
	sentence[i] = '\0';

    return status;
}

void getGPSField(char* sentence, char* buffer, int index)
{
    int sentencePos = 0;
    int fieldPos = 0;
    int commaCount = 0;
	//only 70 characters in the sentence 
    while (sentencePos < 70) {
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



/* old functions

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

*/