#include "gtest/gtest.h"

#include <vector>

#include "tasks/led_blinker.hpp"

static std::vector<Hal::PinDigital> write_log;

/*Fake code from HAL */
void Hal::digitalWrite(Pin pin, Hal::PinDigital val) {
    EXPECT_EQ(pin, Pin::BUILTIN_LED);
    write_log.push_back(val);
}

TEST(LedBlinker, Blinker) {
    write_log.clear();

    for (int i = 0; i < 10; ++i) {
        LEDBlinker::toggle(nullptr);
    }
    ASSERT_EQ(write_log.size(), 10);
    for (int i = 0; i < 10; ++i) {
        if (i % 2 == 0) { // starts with high
            EXPECT_EQ(write_log[i], Hal::PinDigital::VOLTAGE_HIGH);
        } else {
            EXPECT_EQ(write_log[i], Hal::PinDigital::VOLTAGE_LOW);
        }
    }
}
