#include <Arduino.h>
#include "HAL/gpio.h"

namespace Hal{

void pinMode(uint8_t pin, PinMode mode){
    ::pinMode(pin, static_cast<uint8_t>(mode));
}

void digitalWrite(uint8_t pin, PinDigital val){
    ::digitalWrite(pin, static_cast<uint8_t>(val));
}

int analogRead(uint8_t pin){
    return ::analogRead(pin);
}
} //namespace Hal
