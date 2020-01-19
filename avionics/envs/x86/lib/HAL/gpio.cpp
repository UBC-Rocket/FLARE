#include "HAL/gpio.h"

namespace Hal{

void pinMode(uint8_t pin, PinMode mode){
    //TODO - Maybe report this somehow?
}

void digitalWrite(uint8_t pin, PinDigital val){
    //TODO - Maybe report this somehow?
}

int analogRead(uint8_t pin){
    //TODO - Maybe report this somehow?
    return 0;
}

int LED_BUILTIN() {
    return 13;
}

} //namespace Hal
