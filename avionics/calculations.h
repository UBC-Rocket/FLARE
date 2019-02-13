/*Sensors Header*/
#ifndef CALCULATIONS_H
#define CALCULATIONS_H

/*Includes------------------------------------------------------------*/
#include <stdint.h>
#include "sensors.h"

/*Constants------------------------------------------------------------*/

#define MILLISECONDS            1000

/*Variables------------------------------------------------------------*/

/*Functions------------------------------------------------------------*/

void calculateValues(float*, float*, float*, float*, float*, float*, float*, unsigned long*, float*);
void addToPressureSet(float*, float);
float calculatePressureAverage(float*);

#endif
