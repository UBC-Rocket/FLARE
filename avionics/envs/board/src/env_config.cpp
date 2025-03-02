#include "env_config.h"

void env_initialize() {
    // Should be handled by compiler flags now
    // Wire.setSCL(10); // PB6
    // Wire.setSDA(21); // PB7
    Wire.begin();
}
