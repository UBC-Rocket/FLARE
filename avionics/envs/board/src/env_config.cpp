#include "env_config.h"

void env_initialize() {
    Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_EXT, I2C_RATE_400);
    Wire.setDefaultTimeout(100000); // 100ms
}
