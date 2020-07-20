#include "HAL/time.h"
#include "stdio_controller.hpp"

void env_initialize() {
    Hal::initialSystem();
    StdIoController::initialize();
}
