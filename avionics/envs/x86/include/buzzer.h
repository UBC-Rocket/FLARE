#pragma once

#include "buzzer_interface.h"
#include "stdio_controller.hpp"

class Buzzer : public IBuzzer {
  public:
    void sing(SongTypes song) const {
        uint8_t tmp = 1;
        uint8_t tmpsong = static_cast<uint8_t>(song);
        StdIoController::putPacket(0x07, &tmpsong, tmp);
    }

  static void landBuzzer(void *self){
    reinterpret_cast<Buzzer *>(self)->sing(SongTypes_SUCCESS);
  }
};
