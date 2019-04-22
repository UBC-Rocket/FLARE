/*GPIO Header*/
#ifndef GPIO_H
#define GPIO_H

/*Includes------------------------------------------------------------*/
#include <Arduino.h>
#include "sensors.h" //for macros
#include "buzzer.h"
/*Constants------------------------------------------------------------*/

//Double, triple check pin assignments
#define IGNITOR_PIN 16
#define DROGUE_IGNITOR_PIN 17

#define SERVO_PIN 2
#define MELODY_PIN 3

#define CONTINUITY_CHECK_PIN 4
#define CONTINUITY_CHECK_ADC 14
#define DROGUE_CONTINUITY_CHECK_PIN 34
#define DROGUE_CONTINUITY_CHECK_ADC 35

#define POWER_STATUS_LED 20
#define FLIGHT_LED   21

#define INIT_SERVO_POS  60
#define FINAL_SERVO_POS 160

#define IGNITOR_DELAY 15    // in milliseconds
#define SERVO_DELAY 1500    // in miliseconds
#define CONTINUITY_CHECK_DELAY 45 // in microseconds!

// tested at 11.1V with a 2.2k/470 divider (606)
#define DISCONTINUOUS_THRESHOLD 500 // threshold for a discontinuous ematch -> read voltage / 3.3 * 1023

/*Variables------------------------------------------------------------*/

/*Functions------------------------------------------------------------*/
void initPins(void);
void deployDrogue(void);
void deployMain(void);
bool continuityCheck(void);

#endif
