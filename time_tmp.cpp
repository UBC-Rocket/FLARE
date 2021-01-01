#include "time_tmp.h"
namespace Hal {

uint32_t millis() {
    return std::chrono::system_clock::now().time_since_epoch().count();
}
} // namespace Hal
