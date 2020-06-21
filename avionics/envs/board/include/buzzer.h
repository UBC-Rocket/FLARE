/*
 * Buzzer Header
 *
 * @file    buzzer.h
 * @author  UBC Rocket Avionics 2018/2019
 * @description   Initializes buzzer pins and plays melodies using
 *      the buzzer.  These are blocking functions.
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

/*Includes------------------------------------------------------------*/
#include "HAL/pin_util.h"
#include "HAL/time.h"
#include "buzzer_interface.h"

#include "gpio.h"
#undef INPUT
#undef OUTPUT

/*Classes--------------------------------------------------------------*/
class Buzzer : public IBuzzer {
  public:
    /**
     * @brief constructor
     * @param MELODY_PIN I/O pin the buzzer is hooked up to.
     */
    Buzzer() : M_MELODY_PIN(MELODY_PIN) {
        Hal::pinMode(MELODY_PIN, Hal::PinMode::OUTPUT);
    }

    /** void sing(SongTypes song){}
     * @brief  Calculates current values
     * @param  SongTypes song - ID of the melody to be played, defined in
     * buzzer.h
     * @return void.
     */
    void sing(SongTypes song) const;

    /**
     * @brief  Creates a buzzer note at a specified frequency and duration -
     * note that this is a blocking function.
     * @param  long frequency - frequency of the note to be played
     * @param  long length - length of note to be played. To calculate the note
     *          duration, take one second divided by the note type.
     *          e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
     * @return void.
     */
    void buzz(long frequency, long length) const;

  private:
    const int M_MELODY_PIN;
};
