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

enum class PinDigital { LOW = 0, HIGH = 1 };

void pinMode(::Pin pin, PinMode mode);
void digitalWrite(::Pin pin, PinDigital val);
int analogRead(::Pin pin);

int LED_BUILTIN(); // function because c++ doesn't have forward declaration of
                   // variables or something similar
} // namespace Hal
