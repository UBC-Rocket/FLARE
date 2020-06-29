#include <Arduino.h>
// constexpr static int ARDUIN_BUILTIN_LED =
//     LED_BUILTIN; // need to be in front of gpio.h b/c the undef is done there

#include "HAL/pin_util.h"

namespace Hal {

void pinMode(::Pin pin, PinMode mode) {
    ::pinMode(static_cast<uint8_t>(pin), static_cast<uint8_t>(mode));
}

void digitalWrite(::Pin pin, PinDigital val) {
    ::digitalWrite(static_cast<uint8_t>(pin), static_cast<uint8_t>(val));
}

int analogRead(::Pin pin) { return ::analogRead(static_cast<uint8_t>(pin)); }

// int LED_BUILTIN() { return ARDUIN_BUILTIN_LED; }

} // namespace Hal
