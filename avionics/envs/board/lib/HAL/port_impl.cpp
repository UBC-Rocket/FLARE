#include "HAL/port_impl.h"

namespace Hal {

// HardwareSerial CustomSerial1(PC_0, PC_1);
HardwareSerial CustomSerial1(PA_10, PA_9);
HardwareSerial CustomSerial2(3, 2);
HardwareSerial CustomSerial3(PC_15, PC_15);

CustomSerial SerialInst::GPS{CustomSerial1};
CustomSerial SerialInst::Radio{CustomSerial2};
CustomSerial SerialInst::Camera{CustomSerial3};
} // namespace Hal
