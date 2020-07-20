#include "HAL/port_impl.h"

namespace Hal {

Serial SerialInst::GPS{Serial1};
Serial SerialInst::Radio{Serial2};
Serial SerialInst::Camera{Serial3};
} // namespace Hal
