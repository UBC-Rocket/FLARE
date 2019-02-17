/*GPIO Header*/
#ifndef GPIO_H
#define GPIO_H

/*Includes------------------------------------------------------------*/
#include <Arduino.h>

/*Constants------------------------------------------------------------*/
#define IGNITOR_PIN 5
#define SERVO_PIN 30
#define CONTINUITY_CHECK_PIN 6 // I DONT KNOW!!
#define CONTINUITY_CHECK_ADC 7 // I DONT KNOW!!

#define INIT_SERVO_POS  60
#define FINAL_SERVO_POS 160

#define IGNITOR_DELAY 10    // in milliseconds
#define SERVO_DELAY 1000
#define CONTINUITY_CHECK_DELAY 30 // in microseconds!

// tested at 11.1V with a 2.2k/470 divider (606)
#define DISCONTINUOUS_THRESHOLD 500 // threshold for a discontinuous ematch -> read voltage / 3.3 * 1023

/*Variables------------------------------------------------------------*/

/*Functions------------------------------------------------------------*/
void initPins(void);
void deployDrogue(void);
void deployMain(void);
bool continuityCheck(void);

#endif