#ifndef LED_BLINKER_HPP_EA5200B75ECD4CC897C73B0CD1E8BA98
#define LED_BLINKER_HPP_EA5200B75ECD4CC897C73B0CD1E8BA98

#include "HAL/pin_util.h"
#include "HAL/time.h"
#include "gpio.h"

// for some reason these somehow reappear
#undef LOW
#undef HIGH

/**
 * @brief Responsbile for blinking the status LED on non-critical failure.
 */
class LEDBlinker {
  private:
    static bool led_on;

  public:

    /**
     * @brief toggles led
     */
    static void toggle(void *) {
        if (led_on)
            Hal::digitalWrite(Pin::BUILTIN_LED, Hal::PinDigital::HI);
        else
            Hal::digitalWrite(Pin::BUILTIN_LED, Hal::PinDigital::LO);

        led_on = !led_on;
    }

    static constexpr Hal::ms freq{500};
};
#endif
