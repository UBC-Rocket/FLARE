/* @file    gpio.cpp
 * @author  UBC Rocket Avionics 2018/2019
 * @description  Initializes gpio pins that enable us to control our
 *  drogue and main parachute releases.  The drogue servo control
 *  function and main parachute release functions are also contained
 *  here. There is a function that implements continuity testing of the
 *  ematch - required as per IREC regulations.
 *
 * @section LICENSE
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * Distributed as-is; no warranty is given.
 */

/*Includes------------------------------------------------------------*/
#include <Arduino.h>
#include <Servo.h>

#include "gpio.h"

/*Variables------------------------------------------------------------*/
Servo myServo;

/*Functions------------------------------------------------------------*/
/**
  * @brief  Initializes pins for servo (drogue release) and e-match (main release)
  * @param  None
  * @return None
  */
void initPins(void)
{
    /*init ignitor*/
    pinMode(IGNITOR_PIN, OUTPUT);
    digitalWrite(IGNITOR_PIN, LOW);

    #ifdef TESTING
    SerialUSB.println("ignitor pin init low");
    #endif

    /*init ematch continuity check pins */
    pinMode(CONTINUITY_CHECK_PIN, OUTPUT);
    digitalWrite(CONTINUITY_CHECK_PIN, LOW);
    // the ADC read pin does not need to be initialized !!

    #ifdef TESTING
    SerialUSB.println("continuity pins init");
    #endif

    /*init servo*/
    myServo.attach(SERVO_PIN);
    myServo.write(INIT_SERVO_POS);
    #ifdef TESTING
    SerialUSB.println("servo pins init");
    #endif
}

/**
  * @brief  Actuates servo to deploy drogue and payload
  * @param  None
  * @return None
  */
void deployDrogue(void)
{
    myServo.write(FINAL_SERVO_POS);

    #ifdef TESTING
    SerialUSB.println("DROGUE SERVO DEPLOYED");
    #endif

    delay(SERVO_DELAY);
    myServo.write(INIT_SERVO_POS);
}

/**
  * @brief  Ignites e-match for main release
  * @param  None
  * @return None
  */
void deployMain(void)
{
    digitalWrite(IGNITOR_PIN, HIGH);

    #ifdef TESTING
    SerialUSB.println("MAIN IGNITER FIRED");
    #endif

    delay(IGNITOR_DELAY);
    digitalWrite(IGNITOR_PIN, LOW);
}

/** bool continuityCheck(void)
  * @brief  Checks the ematch for continuity
  * @param  None
  * @return bool flag that is true if there is continuity,
  *             false if the ematch is disconnected or broken.
  */
 bool continuityCheck(void){
    digitalWrite(CONTINUITY_CHECK_PIN, HIGH);
    delayMicroseconds(CONTINUITY_CHECK_DELAY);

    int continuity = analogRead(CONTINUITY_CHECK_ADC);
    digitalWrite(CONTINUITY_CHECK_PIN, LOW);

    #ifdef TESTING
    SerialUSB.print("Contiunity check ADC read: ");
    SerialUSB.println(continuity);
    #endif

    if (continuity <= DISCONTINUOUS_THRESHOLD)
        return false;
    else
        return true;

 }
