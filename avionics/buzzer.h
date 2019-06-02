/*Buzzer Header*/
#ifndef BUZZER_H
#define BUZZER_H

/*Includes------------------------------------------------------------*/
#include <Arduino.h>
#include "gpio.h"

enum SongTypes{
  SongTypes_SUCCESS,
  SongTypes_NONCRITFAIL,
  SongTypes_CRITICALFAIL
};

/*Variables------------------------------------------------------------*/

/*Functions------------------------------------------------------------*/
void initBuzzer();
void startBuzzer();
void sing(SongTypes song);
void buzz(int, long, long);

#endif
