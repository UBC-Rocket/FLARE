#pragma once
#include <cstdint>

// NOTE This enum MUST match that defined on the ground station
enum class EventId : uint16_t {
    IGNITOR_FIRED,
    LOW_VOLTAGE
};
