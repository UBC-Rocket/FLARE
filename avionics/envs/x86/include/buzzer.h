#ifndef BUZZER_H_99220B71186147D79C39B54263C9775C
#define BUZZER_H_99220B71186147D79C39B54263C9775C

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
#endif
