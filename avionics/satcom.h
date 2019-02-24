/*Sensors Header*/
#ifndef SATCOM_H
#define SATCOM_H

/*Includes------------------------------------------------------------*/
#include <stdint.h>

// #include "statemachine.h"

/*Constants------------------------------------------------------------*/
#define IridiumSerial Serial3
// #define DIAGNOSTICS // testing
#define BUFSIZE 100

/*Variables------------------------------------------------------------*/

/*Functions------------------------------------------------------------*/
bool SatComSetup(void);
int SatComQuality(void);
void SatComSend(unsigned long*, float*);

#endif