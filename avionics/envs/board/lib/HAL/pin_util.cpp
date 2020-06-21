#include <Arduino.h>
constexpr static int ARDUIN_BUILTIN_LED =
    LED_BUILTIN; // need to be in front of gpio.h b/c the undef is done there

#include "HAL/pin_util.h"

namespace Hal {

void pinMode(uint8_t pin, PinMode mode) {
    ::pinMode(pin, static_cast<uint8_t>(mode));
}

void digitalWrite(uint8_t pin, PinDigital val) {
    ::digitalWrite(pin, static_cast<uint8_t>(val));
}

int analogRead(uint8_t pin) { return ::analogRead(pin); }

int LED_BUILTIN() { return ARDUIN_BUILTIN_LED; }

} // namespace Hal
