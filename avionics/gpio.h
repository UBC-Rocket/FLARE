/*GPIO Header*/
#ifndef GPIO_H
#define GPIO_H

/*Includes------------------------------------------------------------*/
#include <Arduino.h>

/*Constants------------------------------------------------------------*/

//Double, triple check pin assignments
#define IGNITOR_PIN 4
#define SERVO_PIN 2
#define CONTINUITY_CHECK_PIN 16
#define CONTINUITY_CHECK_ADC 14
#define FlightLED   21

#define INIT_SERVO_POS  60
#define FINAL_SERVO_POS 160

#define IGNITOR_DELAY 12    // in milliseconds
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