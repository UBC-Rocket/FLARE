/*
 * GPIO Source
 *
 * @file    gpio.cpp
 * @author  UBC Rocket Avionics 2018/2019
 * @description  Initializes gpio pins that enable us to control our
 *  drogue and main parachute releases, status LEDs, buzzer.
 *
 *  The drogue servo control function and main parachute release functions are
 *  also contained here. There is a function that im plements continuity testing
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
// #include <Servo.h> //TODO - figure out if we want to keep this
#include "HAL/pin_util.h"
#include "HAL/time.h"

#include "buzzer.h"
#include "gpio.h"
#include "options.h"

/*Constants------------------------------------------------------------*/

// #define INIT_SERVO_POS 60
// #define FINAL_SERVO_POS 160

// #define DROGUE_IGNITOR_DELAY 25 // in milliseconds
// #define IGNITOR_DELAY 20
// #define SERVO_DELAY 525           // in miliseconds
// #define CONTINUITY_CHECK_DELAY 50 // in microseconds!

// tested at 11.1V with a 2.2k/470 divider (606) or 1.3k/330 (642)
//  or 1.33k/330 (629)
// threshold for a discontinuous ematch -> read voltage / 3.3 * 1023
// #define DISCONTINUOUS_THRESHOLD 650

/*Variables------------------------------------------------------------*/
// static Servo myServo;

/*Functions------------------------------------------------------------*/
/**
 * @brief  Initializes pins for servo (drogue release), e-match (main release),
 *         LEDs, buzzer
 * @param  None
 * @return None
 */
void initPins(void) {
    /* Initialize and startup power status LEDs */
    Hal::pinMode(Pin::POWER_LED, Hal::PinMode::OUTPUT);
    Hal::digitalWrite(Pin::POWER_LED, Hal::PinDigital::HI);

    /* Initialize and startup flight status LEDs */
    Hal::pinMode(Pin::FLIGHT_LED, Hal::PinMode::OUTPUT);
    Hal::digitalWrite(Pin::FLIGHT_LED, Hal::PinDigital::LO);

    /* Initialize status LED - writing is performed in acutal status evaluation
     */
    Hal::pinMode(Pin::STATUS_LED, Hal::PinMode::OUTPUT);
    Hal::pinMode(Pin::BUILTIN_LED, Hal::PinMode::OUTPUT);

    /* Initialize ignitor pins to be off. This also exists in ignitor but may be called too late? */
    Hal::pinMode(Pin::DROGUE_IGNITOR, Hal::PinMode::OUTPUT);
    Hal::digitalWrite(Pin::DROGUE_IGNITOR, Hal::PinDigital::LO);
    Hal::pinMode(Pin::MAIN_IGNITOR, Hal::PinMode::OUTPUT);
    Hal::digitalWrite(Pin::MAIN_IGNITOR, Hal::PinDigital::LO);

    // Ignitor and continuity pins are initialized in the ignitor constructor

    //     /*init main ignitor*/
    //     Hal::pinMode(Pin::IGNITOR, Hal::PinMode::OUTPUT);
    //     Hal::digitalWrite(Pin::IGNITOR, Hal::PinDigital::LO);

    // #ifdef TESTING
    //     SerialUSB.println("main ignitor pin init low");
    // #endif

    //     /*init ematch continuity check pins */
    //     Hal::pinMode(Pin::MAIN_CONTINUITY_TEST, Hal::PinMode::OUTPUT);
    //     Hal::digitalWrite(Pin::MAIN_CONTINUITY_TEST, Hal::PinDigital::LO);
    //     // the ADC read pin does not need to be initialized !!

    // #ifdef TESTING
    //     SerialUSB.println("main continuity pins init");
    // #endif

    //     /*init drogue ignitor*/
    //     Hal::pinMode(Pin::DROGUE_IGNITOR, Hal::PinMode::OUTPUT);
    //     Hal::digitalWrite(Pin::DROGUE_IGNITOR, Hal::PinDigital::LO);

    // #ifdef TESTING
    //     SerialUSB.println("drogue ignitor pin init low");
    // #endif

    //     /*init drogue ematch continuity check pins */
    //     Hal::pinMode(Pin::DROGUE_CONTINUITY_TEST, Hal::PinMode::OUTPUT);
    //     Hal::digitalWrite(Pin::DROGUE_CONTINUITY_TEST, Hal::PinDigital::LO);

    // #ifdef TESTING
    //     SerialUSB.println("drogue continuity pins init");
    // #endif
}

// /**
//  * @brief  Deploys drogue and payload
//  * @param  None
//  * @return None
//  */
// void deployDrogue(void) {
//     // #ifdef SERVO
//     //     myServo.write(FINAL_SERVO_POS);

//     //     #ifdef TESTING
//     //     SerialUSB.println("DROGUE SERVO DEPLOYED");
//     //     #endif

//     //     Hal::sleep_ms(SERVO_DELAY);
//     //     myServo.write(INIT_SERVO_POS);
//     // #endif  // SERVO

// #ifdef POW
//     Hal::digitalWrite(DROGUE_IGNITOR_PIN, Hal::PinDigital::HI);

// #ifdef TESTING
//     SerialUSB.println("DROGUE IGNITER FIRED");
// #endif

//     Hal::sleep_ms(DROGUE_IGNITOR_DELAY);
//     Hal::digitalWrite(DROGUE_IGNITOR_PIN, Hal::PinDigital::LO);
// #endif // POW
// }

// /**
//  * @brief  Ignites e-match for main release
//  * @param  None
//  * @return None
//  */
// void deployMain(void) {
//     Hal::digitalWrite(IGNITOR_PIN, Hal::PinDigital::HI);

// #ifdef TESTING
//     SerialUSB.println("MAIN IGNITER FIRED");
// #endif

//     Hal::sleep_ms(IGNITOR_DELAY);
//     Hal::digitalWrite(IGNITOR_PIN, Hal::PinDigital::LO);
// }

/** bool continuityCheck(void)
 * @brief  Checks the ematches for continuity
 * @param  None
 * @return bool flag that is true if there is continuity,
 *             false if the ematch is disconnected or broken.
 */
// bool continuityCheck(void) {
//     Hal::digitalWrite(CONTINUITY_CHECK_PIN, Hal::PinDigital::HI);
//     Hal::sleep_us(CONTINUITY_CHECK_DELAY);

//     int main_continuity = Hal::analogRead(CONTINUITY_CHECK_ADC);
//     Hal::digitalWrite(CONTINUITY_CHECK_PIN, Hal::PinDigital::LO);

// #ifdef TESTING
//     SerialUSB.print("Main contiunity check ADC read: ");
//     SerialUSB.println(main_continuity);
// #endif

// #ifdef POW
//     Hal::digitalWrite(DROGUE_CONTINUITY_CHECK_PIN, Hal::PinDigital::HI);
//     Hal::sleep_us(CONTINUITY_CHECK_DELAY);

//     int drogue_continuity = Hal::analogRead(DROGUE_CONTINUITY_CHECK_ADC);
//     Hal::digitalWrite(DROGUE_CONTINUITY_CHECK_PIN, Hal::PinDigital::LO);

// #ifdef TESTING
//     SerialUSB.print("Drogue contiunity check ADC read: ");
//     SerialUSB.println(drogue_continuity);
// #endif
// #endif // POW

//     if (main_continuity <= DISCONTINUOUS_THRESHOLD)
//         return false;
// #ifdef POW
//     if (drogue_continuity <= DISCONTINUOUS_THRESHOLD)
//         return false;
// #endif // POW

//     return true;
// }
