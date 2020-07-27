#include "HAL/port_impl.h"
#include "HAL/time.h"
#include "stdio_controller.hpp"

void env_initialize() {
    Hal::initialSystem();
    StdIoController::initialize();
}

void env_callbacks() { Hal::Serial::watchdog_callback(); }
