/*Ground Altitude Calculation Header*/
#ifndef GROUNDALTITUDE_H
#define GROUNDALTITUDE_H

/*Includes------------------------------------------------------------*/
#include <stdint.h>

/*Constants------------------------------------------------------------*/
#define GROUND_ALT_SIZE 20  //array size for moving average

/*Variables------------------------------------------------------------*/

/*Functions------------------------------------------------------------*/
// float groundAlt_init(float*);
float groundAlt_arr_sum(float*);
float groundAlt_update(float*, float*);

#endif
