/*GPIO Header*/
#ifndef GPIO_H
#define GPIO_H

/*Includes------------------------------------------------------------*/
#include <Arduino.h>
#include "sensors.h" //for macros
#include "buzzer.h"

/*Constants------------------------------------------------------------*/
#define POWER_STATUS_LED 20 //kept in one place w/ FLIGHT_LED
#define FLIGHT_LED   21 //required by state machine

#define MELODY_PIN 3 //required by buzzer

/*Variables------------------------------------------------------------*/

/*Functions------------------------------------------------------------*/
void initPins(void);
void deployDrogue(void);
void deployMain(void);
bool continuityCheck(void);

#endif
