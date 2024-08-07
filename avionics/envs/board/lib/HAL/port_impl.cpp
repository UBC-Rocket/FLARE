#include "HAL/port_impl.h"

namespace Hal {

HardwareSerial CustomSerial1(197, 196); // RX: PC0, TX: PC1
HardwareSerial CustomSerial2(193, 192); // RX: PA1, TX: PA0
HardwareSerial CustomSerial3(194, 194); // RX: PA4 (unused), TX: PA4 (unused)

CustomSerial SerialInst::GPS{CustomSerial1};
CustomSerial SerialInst::Radio{CustomSerial2};
CustomSerial SerialInst::Camera{CustomSerial3};
} // namespace Hal
