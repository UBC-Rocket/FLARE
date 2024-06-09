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
    // POWER_LED = 0,  // N/A on Rev 2 board
    // FLIGHT_LED = 0, // N/A on Rev 2 board
    BUILTIN_LED = 33, // PC6
    STATUS_LED = 33, // PC6, uses BUILTIN_LED
    BUZZER = 41, // PB15
    MAIN_IGNITOR = 14, // PB9, TODO!
    MAIN_CONTINUITY_TEST = 2, // PA10, TODO!
    MAIN_CONTINUITY_READ = 2, // PA10, TODO!
    DROGUE_IGNITOR = 15, // PB8, TODO!
    DROGUE_CONTINUITY_TEST = 8, // PA9, TODO!
    DROGUE_CONTINUITY_READ = 8, // PA9, TODO!
    VOLTAGE_SENSOR = 42, // PB14, unused
    SD_CHIP_SELECT = 31, // PC9
};

/*Functions------------------------------------------------------------*/
void initPins(void);
// void deployDrogue(void);
// void deployMain(void);
// bool continuityCheck(void);
