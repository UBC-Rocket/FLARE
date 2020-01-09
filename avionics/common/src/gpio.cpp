/*
 * GPIO Source
 *
 * @file    gpio.cpp
 * @author  UBC Rocket Avionics 2018/2019
 * @description  Initializes gpio pins that enable us to control our
 *  drogue and main parachute releases, status LEDs, buzzer.
 *
 *  The drogue servo control function and main parachute release functions are
 *  also contained here. There is a function that implements continuity testing
 *  of the ematch - required as per IREC regulations.
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
#include <Servo.h>
#include "HAL/gpio.h"
#include "HAL/time.h"

#include "buzzer.h"
#include "gpio.h"
#include "options.h"

/*Constants------------------------------------------------------------*/

//Double, triple check pin assignments
#define IGNITOR_PIN 16
#define DROGUE_IGNITOR_PIN 35

#define SERVO_PIN 2

#define CONTINUITY_CHECK_PIN 4
#define CONTINUITY_CHECK_ADC 14
#define DROGUE_CONTINUITY_CHECK_PIN 17
#define DROGUE_CONTINUITY_CHECK_ADC 34

#define INIT_SERVO_POS  60
#define FINAL_SERVO_POS 160

#define DROGUE_IGNITOR_DELAY 25    // in milliseconds
#define IGNITOR_DELAY 20
#define SERVO_DELAY 525    // in miliseconds
#define CONTINUITY_CHECK_DELAY 50 // in microseconds!

//tested at 11.1V with a 2.2k/470 divider (606) or 1.3k/330 (642)
//  or 1.33k/330 (629)
// threshold for a discontinuous ematch -> read voltage / 3.3 * 1023
#define DISCONTINUOUS_THRESHOLD 650

/*Variables------------------------------------------------------------*/
static Servo myServo;

/*Functions------------------------------------------------------------*/
/**
  * @brief  Initializes pins for servo (drogue release), e-match (main release),
  *         LEDs, buzzer
  * @param  None
  * @return None
  */
void initPins(void)
{
    /* Initialize and startup power status LEDs*/
    Hal::pinMode(POWER_STATUS_LED, Hal::PinMode::OUTPUT);
    Hal::digitalWrite(POWER_STATUS_LED, Hal::PinDigital::HIGH);

    /* Initialize and startup flight status LEDs*/
    Hal::pinMode(FLIGHT_LED, Hal::PinMode::OUTPUT);
    Hal::digitalWrite(FLIGHT_LED, Hal::PinDigital::LOW);

    #ifdef BODY
        /*init main ignitor*/
        Hal::pinMode(IGNITOR_PIN, Hal::PinMode::OUTPUT);
        Hal::digitalWrite(IGNITOR_PIN, Hal::PinDigital::LOW);

        #ifdef TESTING
        SerialUSB.println("main ignitor pin init low");
        #endif

        /*init ematch continuity check pins */
        Hal::pinMode(CONTINUITY_CHECK_PIN, Hal::PinMode::OUTPUT);
        Hal::digitalWrite(CONTINUITY_CHECK_PIN, Hal::PinDigital::LOW);
        // the ADC read pin does not need to be initialized !!

        #ifdef TESTING
        SerialUSB.println("main continuity pins init");
        #endif

        #ifdef POW
            /*init drogue ignitor*/
            Hal::pinMode(DROGUE_IGNITOR_PIN, Hal::PinMode::OUTPUT);
            Hal::digitalWrite(DROGUE_IGNITOR_PIN, Hal::PinDigital::LOW);

            #ifdef TESTING
            SerialUSB.println("drogue ignitor pin init low");
            #endif

            /*init drogue ematch continuity check pins */
            Hal::pinMode(DROGUE_CONTINUITY_CHECK_PIN, Hal::PinMode::OUTPUT);
            Hal::digitalWrite(DROGUE_CONTINUITY_CHECK_PIN, Hal::PinDigital::LOW);

            #ifdef TESTING
            SerialUSB.println("drogue continuity pins init");
            #endif
        #endif // POW

        #ifdef SERVO
            /*init servo*/
            myServo.attach(SERVO_PIN);
            myServo.write(INIT_SERVO_POS);
            #ifdef TESTING
            SerialUSB.println("servo pins init");
            #endif
        #endif // SERVO

    #endif //body

}

/**
  * @brief  Deploys drogue and payload
  * @param  None
  * @return None
  */
void deployDrogue(void)
{
    #ifdef SERVO
        myServo.write(FINAL_SERVO_POS);

        #ifdef TESTING
        SerialUSB.println("DROGUE SERVO DEPLOYED");
        #endif

        Hal::sleep_ms(SERVO_DELAY);
        myServo.write(INIT_SERVO_POS);
    #endif  // SERVO

    #ifdef POW
        Hal::digitalWrite(DROGUE_IGNITOR_PIN, Hal::PinDigital::HIGH);

        #ifdef TESTING
        SerialUSB.println("DROGUE IGNITER FIRED");
        #endif

        Hal::sleep_ms(DROGUE_IGNITOR_DELAY);
        Hal::digitalWrite(DROGUE_IGNITOR_PIN, Hal::PinDigital::LOW);
    #endif // POW
}

/**
  * @brief  Ignites e-match for main release
  * @param  None
  * @return None
  */
void deployMain(void)
{
    Hal::digitalWrite(IGNITOR_PIN, Hal::PinDigital::HIGH);

    #ifdef TESTING
    SerialUSB.println("MAIN IGNITER FIRED");
    #endif

    Hal::sleep_ms(IGNITOR_DELAY);
    Hal::digitalWrite(IGNITOR_PIN, Hal::PinDigital::LOW);
}

/** bool continuityCheck(void)
  * @brief  Checks the ematches for continuity
  * @param  None
  * @return bool flag that is true if there is continuity,
  *             false if the ematch is disconnected or broken.
  */
 bool continuityCheck(void){
    Hal::digitalWrite(CONTINUITY_CHECK_PIN, Hal::PinDigital::HIGH);
    Hal::sleep_us(CONTINUITY_CHECK_DELAY);

    int main_continuity = Hal::analogRead(CONTINUITY_CHECK_ADC);
    Hal::digitalWrite(CONTINUITY_CHECK_PIN, Hal::PinDigital::LOW);

    #ifdef TESTING
    SerialUSB.print("Main contiunity check ADC read: ");
    SerialUSB.println(main_continuity);
    #endif

    #ifdef POW
        Hal::digitalWrite(DROGUE_CONTINUITY_CHECK_PIN, Hal::PinDigital::HIGH);
        Hal::sleep_us(CONTINUITY_CHECK_DELAY);

        int drogue_continuity = Hal::analogRead(DROGUE_CONTINUITY_CHECK_ADC);
        Hal::digitalWrite(DROGUE_CONTINUITY_CHECK_PIN, Hal::PinDigital::LOW);

        #ifdef TESTING
        SerialUSB.print("Drogue contiunity check ADC read: ");
        SerialUSB.println(drogue_continuity);
        #endif
    #endif // POW

    if (main_continuity <= DISCONTINUOUS_THRESHOLD)
        return false;
    #ifdef POW
        if(drogue_continuity <= DISCONTINUOUS_THRESHOLD)
            return false;
    #endif  // POW

    return true;

 }
