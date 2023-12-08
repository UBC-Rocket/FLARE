#include "HAL/port_impl.h"

namespace Hal {

HardwareSerial CustomSerial1(10, 9); // PA_10, PA_9
HardwareSerial CustomSerial2(3, 2); // PA_3, PA_2
HardwareSerial CustomSerial3(47, 47); // PC_15 (unused)

CustomSerial SerialInst::GPS{CustomSerial1};
CustomSerial SerialInst::Radio{CustomSerial2};
CustomSerial SerialInst::Camera{CustomSerial3};
} // namespace Hal
