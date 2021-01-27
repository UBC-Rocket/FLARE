#pragma once
#include <cstdint>

// NOTE This enum MUST match that defined on the ground station
enum class EventId : uint16_t {
    STANDBY,
    ARMED,
    POWERED_ASCENT,
    PRE_AIR_START_COAST_TIMED,
    ASCENT_TO_APOGEE,
    MACH_LOCK,
    PRESSURE_DELAY,
    DROGUE_DESCENT,
    MAIN_DESCENT,
    LANDED,
    WINTER_CONTINGENCY,

    IGNITOR_FIRED
};
