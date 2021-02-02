#pragma once

// workaround Arduino's framework that pre-processor defines input & output
#undef INPUT
#undef OUTPUT
#undef LOW
#undef HIGH
#undef LED_BUILTIN

#include "gpio.h"
#include <cstdint>

namespace Hal {
enum class PinMode { INPUT = 0, OUTPUT = 1 };

// TODO HIGH and LOW conflict with existing macro. We should adopt a better
// naming convention for enums.
enum class PinDigital { LO = 0, HI = 1 };

void pinMode(::Pin pin, PinMode mode);
void digitalWrite(::Pin pin, PinDigital val);
int analogRead(::Pin pin);

// int LED_BUILTIN(); // function because c++ doesn't have forward declaration
// of variables or something similar
} // namespace Hal
