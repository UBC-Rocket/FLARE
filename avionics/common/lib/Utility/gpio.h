#ifndef UTILITY__GPIO_H_41B1B7824F0141F8A5A03070A0D0A3F3
#define UTILITY__GPIO_H_41B1B7824F0141F8A5A03070A0D0A3F3

//workaround Arduino's framework that pre-processor defines input & output
#undef INPUT
#undef OUTPUT
#undef LOW
#undef HIGH

namespace Utility {
    enum class PinMode {
        INPUT = 0,
        OUTPUT = 1
    };

    enum class PinDigital {
        LOW = 0,
        HIGH = 1
    };

    void pinMode(uint8_t pin, PinMode mode);

    void digitalWrite(uint8_t pin, PinDigital val);
}

#endif
