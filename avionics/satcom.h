/*Sensors Header*/
#ifndef SATCOM_H
#define SATCOM_H

/*Includes------------------------------------------------------------*/
#include <stdint.h>

// #include "statemachine.h"

/*Constants------------------------------------------------------------*/
#define IridiumSerial Serial4
#define SAT_COM_DATA_ARRAY_SIZE 34
// #define DIAGNOSTICS // testing
#define BUFSIZE 100
#define NUM_SATCOM_SENDS_ON_LANDED 3
#define SATCOM_LANDED_TIME_INTERVAL 60000 //in ms

/*Variables------------------------------------------------------------*/

/*Functions------------------------------------------------------------*/
bool SatComSetup(void);
int SatComQuality(void);
void SatComSendGPS(unsigned long*, float*);
bool SatComReceive(char*);

#endif
