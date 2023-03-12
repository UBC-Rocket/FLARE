#include "HAL/port_impl.h"

namespace Hal {

HardwareSerial CustomSerial1(PC_0, PC_1);
HardwareSerial CustomSerial2(PC_4, PC_5);
HardwareSerial CustomSerial3(PA_3, PA_2);

CustomSerial SerialInst::GPS{CustomSerial1};
CustomSerial SerialInst::Radio{CustomSerial2};
CustomSerial SerialInst::Camera{CustomSerial3};
} // namespace Hal
