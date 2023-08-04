#include <cstdint>
#include <ctime>
#include <fstream>
#include <string>

#include "HAL/port_impl.h"
#include "HAL/time.h"
#include "log.hpp"
#include "stdio_controller.hpp"

static std::ofstream output_file;
static rktlog::OstreamOutput output_stream(output_file);

// TODO - rocket ID is hijacked to give different excecutables different debug
// log file names. As such the rocket ID macro should probably be renamed; also,
// to keep it more explicit it's probably better to put it into the various
// config.h files around
#ifndef RADIO_CONFIG_PACKET_ROCKET_ID
#define RADIO_CONFIG_PACKET_ROCKET_ID 255
#endif

void env_initialize() {
    std::string logfile_name("debuglog_");
    char datetime_buf[50];
    std::time_t time = std::time(nullptr);
    std::strftime(datetime_buf, 50, "%Y-%m-%dT%H-%M-%SZ",
                  std::localtime(&time));

    logfile_name += std::to_string(RADIO_CONFIG_PACKET_ROCKET_ID);
    logfile_name += "_";
    logfile_name += datetime_buf;
    output_file.open(logfile_name, std::ios_base::out | std::ios_base::app);

    rktlog::Logger::setOutput(output_stream);

    StdIoController::initialize();
}

void env_callbacks() { Hal::CustomSerial::watchdog_callback(); }
