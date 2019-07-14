/*
 * Buzzer Header
 *
 * @file    buzzer.h
 * @author  UBC Rocket Avionics 2018/2019
 * @description   Initializes buzzer pins and plays melodies using
 *      the buzzer.  These are blocking functions.
 *
 * @section LICENSE
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * Distributed as-is; no warranty is given.
 */

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
