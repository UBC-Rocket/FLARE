#include "HAL/pin_util.h"
#include "stdio_controller.hpp"

namespace Hal {

void pinMode(Pin pin, PinMode mode) {
    uint8_t tmp[2];
    tmp[0] = static_cast<uint8_t>(pin);
    tmp[1] = static_cast<uint8_t>(mode);
    StdIoController::putPacket(0x4D, tmp, 2); 
    
}

void digitalWrite(Pin pin, PinDigital val) {
    uint8_t tmp[2];
    tmp[0] = static_cast<uint8_t>(pin);
    tmp[1] = static_cast<uint8_t>(val);
    StdIoController::putPacket(0x50, tmp, 2); // 0x50 = P, see multiplexing spec
}

int analogRead(Pin pin) {
    return StdIoController::requestAnalogRead(static_cast<uint8_t>(pin));
}

// int LED_BUILTIN() { return 13; }

} // namespace Hal
