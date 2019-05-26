/*Buzzer Header*/
#ifndef BUZZER_H
#define BUZZER_H

/*Includes------------------------------------------------------------*/
#include <Arduino.h>
#include "gpio.h"

/*Variables------------------------------------------------------------*/
enum SongTypes{
  SongTypes_SUCCESS,
  SongTypes_NONCRITFAIL,
  SongTypes_CRITICALFAIL
};

/*Functions------------------------------------------------------------*/
void initBuzzer();
void startBuzzer();
void sing(SongTypes song);
void buzz(int, long, long);

#endif
