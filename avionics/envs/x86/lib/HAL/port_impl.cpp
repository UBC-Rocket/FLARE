#include "HAL/port_impl.h"
#include <algorithm>

namespace Hal {
std::vector<CustomSerial *> CustomSerial::serials_{};
constexpr std::chrono::milliseconds CustomSerial::MAX_SEND_PERIOD_;

int CustomSerial::read() {
    if (IO_ID_ == StdIoController::DEV_NULL) {
        // TODO log error
        return -1;
    }

    uint8_t c;
    if (StdIO::get(IO_ID_, c))
        return c;
    return -1;
}

std::size_t CustomSerial::write(const uint8_t *const inbuf, std::size_t const size) {
    if (IO_ID_ == StdIoController::DEV_NULL) {
        return size;
    }
    if (size + buf_used_ > BUFFER_SIZE) {
        send_buffer();
    }
    std::copy(inbuf, inbuf + size, buf_.begin() + buf_used_);
    buf_used_ += size;
    return size;
}

CustomSerial SerialInst::USB{StdIoController::DEV_NULL};
CustomSerial SerialInst::GPS{StdIoController::DEV_NULL};
CustomSerial SerialInst::Radio{'R'};
CustomSerial SerialInst::Camera{StdIoController::DEV_NULL};
} // namespace Hal
