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
    POWER_LED = 33,  // kept in one place w/ FLIGHT_LED
    FLIGHT_LED = 33, // required by state machine
    BUILTIN_LED = 31,
    STATUS_LED = 31, // uses BUILTIN_LED
    BUZZER = 25,      // required by buzzer
    MAIN_IGNITOR = 15,
    MAIN_CONTINUITY_TEST = 33,
    MAIN_CONTINUITY_READ = 33,
    DROGUE_IGNITOR = 20,
    DROGUE_CONTINUITY_TEST = 33,
    DROGUE_CONTINUITY_READ = 33,
    VOLTAGE_SENSOR = 33, // PC15 (unused)
    SD_CHIP_SELECT = 196, // PA4
};

/*Functions------------------------------------------------------------*/
void initPins(void);
// void deployDrogue(void);
// void deployMain(void);
// bool continuityCheck(void);
