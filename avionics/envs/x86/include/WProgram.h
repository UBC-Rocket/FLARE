// Dummy file for compilation purposes.
#pragma once
#include "HAL/time.h"

inline uint32_t millis() { return Hal::now_ms().time_since_epoch().count(); }
