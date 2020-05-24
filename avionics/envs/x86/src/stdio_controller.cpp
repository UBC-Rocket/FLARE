#include "stdio_controller.hpp"

std::thread StdIoController::m_input{&StdIoController::inputLoop};
std::unordered_map<StdIoController::Id, std::mutex>
    StdIoController::m_mutexes{};
std::unordered_map<StdIoController::Id, std::queue<uint8_t>>
    StdIoController::m_istreams{};
std::mutex StdIoController::m_cout{};

