#pragma once

#include "buzzer_interface.h"
#include "stdio_controller.hpp"

class Buzzer : public IBuzzer {
  public:
    void sing(SongTypes song) const {
        uint8_t tmp = 1;
        char tmpsong = static_cast<char>(song);
        StdIoController::putPacket(0x07, &tmpsong, tmp);
    }
};
