#include "HAL/port_impl.h"

namespace Hal {

CustomSerial SerialInst::GPS{Serial1};
CustomSerial SerialInst::Radio{Serial2};
CustomSerial SerialInst::Camera{Serial3};
} // namespace Hal
