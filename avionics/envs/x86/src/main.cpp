#include "env_config.h"

int main(void) {
    setup();
    for (;;) {
        loop();
    }
}