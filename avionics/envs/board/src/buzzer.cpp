/*
 * Buzzer Source
 *
 * @file    buzzer.cpp
 * @author  UBC Rocket Avionics 2018/2019
 * @description   Initializes buzzer pins and plays melodies using
 *     the buzzer.  These are blocking functions.
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
#include <chrono>
#include "HAL/time.h"
#include "HAL/gpio.h"

#include "buzzer.h"

/*Constants------------------------------------------------------------*/
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

/*Functions------------------------------------------------------------*/

/* void sing(SongTypes song){}
 * @brief  Calculates current values
 * @param  SongTypes song - ID of the melody to be played, defined in buzzer.h
 * @return void.
 */
void Buzzer::sing(SongTypes song) const {
    switch(song){
        case SongTypes_SUCCESS:
        {
            buzz(NOTE_C1, 500);
            buzz(NOTE_C7, 1000);
            buzz(NOTE_G7, 1000);
            buzz(NOTE_C8, 1000);
            break;
        }
        case SongTypes_NONCRITFAIL:
        {
            buzz(NOTE_C1, 500);
            buzz(NOTE_C6, 300);
            Hal::sleep_ms(300);
            buzz(NOTE_C6, 300);
            Hal::sleep_ms(300);
            buzz(NOTE_C6, 300);
            Hal::sleep_ms(300);
            buzz(NOTE_C6, 300);
            Hal::sleep_ms(300);
            buzz(NOTE_C6, 300);
            Hal::sleep_ms(300);
            buzz(NOTE_C6, 300);
            Hal::sleep_ms(300);
            buzz(NOTE_C6, 300);
            Hal::sleep_ms(300);
            buzz(NOTE_C6, 300);
            Hal::sleep_ms(300);

            break;
        }
        case SongTypes_CRITICALFAIL:
        {
            buzz(NOTE_C1, 300);
            buzz(NOTE_C7, 800);
            buzz(NOTE_B6, 800);
            buzz(NOTE_AS6, 800);
            buzz(NOTE_A6, 2500);
            break;
        }
    }
}


void Buzzer::buzz(long frequency, long length) const {

    long delayValue = 1000000 / frequency / 2; //delay between transitions
    // 1 000 000 microseconds, divided by the frequency, divided by 2 b/c
    // there are two phases to each cycle
    long numCycles = frequency * length / 1000; // #of cycles for proper timing
    // multiply frequency = cycles per second, by the number of seconds to
    // get the total number of cycles to produce
    for (long i = 0; i < numCycles; i++) { // for the calculated length of time
        Hal::digitalWrite(M_MELODY_PIN, Hal::PinDigital::HIGH); // write high to push out the diaphram
        Hal::sleep_us(delayValue); // wait for the calculated delay value
        Hal::digitalWrite(M_MELODY_PIN, Hal::PinDigital::LOW); // write low to pull back the diaphram
        Hal::sleep_us(delayValue); // wait for the calculated delay value
    }
}
