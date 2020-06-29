#pragma once

enum class RocketStatus {
    NOMINAL = 0x00,
    NONCRITICAL_FAILURE = 0x01,
    CRITICAL_FAILURE = 0x03
};

enum class ComponentStatus { NOMINAL = 0x00, FAILURE = 0x01 };

// TODO is this wanted?
typedef ComponentStatus SensorStatus;
typedef ComponentStatus HardwareStatus;
