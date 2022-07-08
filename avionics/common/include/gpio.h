/*
 * GPIO Header
 *
 * @file    gpio.h
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

#pragma once

/*Constants------------------------------------------------------------*/
// Double, triple check pin assignments
enum class Pin {
    POWER_LED = 20,  // kept in one place w/ FLIGHT_LED
    FLIGHT_LED = 21, // required by state machine
    BUILTIN_LED = 13,
    STATUS_LED = 13, // uses BUILTIN_LED
    BUZZER = 23,      // required by buzzer
    MAIN_IGNITOR = 4,
    MAIN_CONTINUITY_TEST = 0, //replaced for tantalus lite
    MAIN_CONTINUITY_READ = 0, //replaced for tantalus lite
    MAIN_REDUNDANT_IGNITOR = 5,
    MAIN_REDUNDANT_CONTINUITY_TEST = 0, //TODO after tantalus
    MAIN_REDUNDANT_CONTINUITY_READ = 0, //TODO after tantalus
    DROGUE_IGNITOR = 2,
    DROGUE_CONTINUITY_TEST = 0, //replaced for tantalus lite
    DROGUE_CONTINUITY_READ = 0, //replaced for tantalus lite
    DROGUE_REDUNDANT_IGNITOR = 3,
    DROGUE_REDUNDANT_CONTINUITY_TEST = 0, //TODO after tantalus
    DROGUE_REDUNDANT_CONTINUITY_READ = 0, //TODO after tantalus
    VOLTAGE_SENSOR = 23
};

/*Functions------------------------------------------------------------*/
void initPins(void);
// void deployDrogue(void);
// void deployMain(void);
// bool continuityCheck(void);
