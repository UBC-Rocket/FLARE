#include "HAL/port_impl.h"
#include <algorithm>

namespace Hal {
std::vector<Serial *> Serial::serials_{};
constexpr std::chrono::milliseconds Serial::MAX_SEND_PERIOD_;

int Serial::read() {
    if (IO_ID_ == StdIoController::DEV_NULL) {
        // TODO log error
        return -1;
    }

    uint8_t c;
    if (StdIO::get(IO_ID_, c))
        return c;
    return -1;
}

std::size_t Serial::write(const uint8_t *const inbuf, std::size_t const size) {
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

Serial SerialInst::USB{StdIoController::DEV_NULL};
Serial SerialInst::GPS{StdIoController::DEV_NULL};
Serial SerialInst::Radio{'R'};
Serial SerialInst::Camera{StdIoController::DEV_NULL};
} // namespace Hal
