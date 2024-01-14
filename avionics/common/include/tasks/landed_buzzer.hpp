/**
 * Landed Buzzer
 *
 * @file landed_buzzer.hpp
 * @author UBC Rocket Avionics 2023/2024
 * @description Class that allows landed buzzer to be run within a task
 */

#pragma once

/*Includes------------------------------------------------------------*/
#include "HAL/time.h"
#include "buzzer.h"

class LandedBuzzer {

  private:
    Buzzer buzzer_;

  public:
    LandedBuzzer(Buzzer &buzzer) : buzzer_(buzzer) {}

    // run function to be called by a task
    static void run(void *self) {
        reinterpret_cast<LandedBuzzer *>(self)->buzzer_.sing(SongTypes_LANDED);
    }
    static constexpr Hal::ms freq{500};
};
