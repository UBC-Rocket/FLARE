#include "HAL/gpio.h"
#include "stdio_controller.hpp"

namespace Hal{

void pinMode(uint8_t pin, PinMode mode){
    //TODO - Maybe report this somehow?
}

void digitalWrite(uint8_t pin, PinDigital val){
    //TODO - Maybe report this somehow?
    uint8_t tmp[] = {pin, static_cast<uint8_t>(val)};
    StdIoController::putPacket(0x50, tmp, 2); // 0x50 = P, see multiplexing spec
}

int analogRead(uint8_t pin){
    //TODO - Maybe report this somehow?
    return 0;
}

int LED_BUILTIN() {
    return 13;
}

} //namespace Hal
