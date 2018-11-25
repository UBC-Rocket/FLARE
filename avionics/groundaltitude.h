/*Ground Altitude Calculation Header*/
#ifndef GROUNDALTITUDE_H
#define GROUNDALTITUDE_H

/*Includes------------------------------------------------------------*/
#include <stdint.h>

/*Constants------------------------------------------------------------*/
#define TESTING //enable or disable debug output

#define ground_alt_delay 100  //in ms
#define ground_alt_size 20  //array size for moving average
#define BASELINE_PRESSURE 1012 //TODO: calibrate

/*Variables------------------------------------------------------------*/

/*Functions------------------------------------------------------------*/
void groundAlt_init(float*, float*, float*, float*,float*);
void groundAlt_update(float*, float*, float*, float*, float*)

#endif
