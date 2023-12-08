#include "env_config.h"

void env_initialize() {
    Wire.setSCL(22); // PB6
    Wire.setSDA(23); // PB7
    Wire.begin();
}
