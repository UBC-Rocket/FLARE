/*
 * GPIO Header
 *
 * @file    gpio.h
 * @author  UBC Rocket Avionics 2018/2019
 * @description  Initializes gpio pins that enable us to control our
 *  drogue and main parachute releases, status LEDs, buzzer.
 *
 *  The drogue servo control function and main parachute release functions are also contained
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

#ifndef GPIO_H
#define GPIO_H

/*Includes------------------------------------------------------------*/
#include <Arduino.h>

/*Constants------------------------------------------------------------*/
#define POWER_STATUS_LED 20 //kept in one place w/ FLIGHT_LED
#define FLIGHT_LED   21 //required by state machine

#define MELODY_PIN 3 //required by buzzer

/*Functions------------------------------------------------------------*/
void initPins(void);
void deployDrogue(void);
void deployMain(void);
bool continuityCheck(void);

#endif
