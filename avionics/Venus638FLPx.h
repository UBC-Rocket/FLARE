/*GPS Venus638FLPx Driver Header*/

#ifndef Venus638FLPx_h
#define Venus638FLPx_h

/*Includes------------------------------------------------------------*/
#include <HardwareSerial.h>

/*Constants------------------------------------------------------------*/
#define SENTENCE_SIZE   60
#define GPS_TIMEOUT     100

/*Variables------------------------------------------------------------*/

/*Functions------------------------------------------------------------*/
bool getGPSData(char* sentence);
void getGPSField(char* sentence, char* buffer, int index);

#endif
