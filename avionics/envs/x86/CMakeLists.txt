# file(GLOB x86_files ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
# file(GLOB x86_HAL ${CMAKE_CURRENT_LIST_DIR}/lib/HAL/*.cpp)

set(x86_specific_files
    ${CMAKE_CURRENT_LIST_DIR}/lib/HAL/pin_util.cpp
    ${CMAKE_CURRENT_LIST_DIR}/lib/HAL/time.cpp
    ${CMAKE_CURRENT_LIST_DIR}/lib/HAL/port_impl.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/stdio_controller.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/env_config.cpp
)

target_sources(
    TantalusStage1
    PRIVATE
        ${x86_specific_files}
)
target_sources(
    TantalusStage2
    PRIVATE
        ${x86_specific_files}
)

target_sources(
    Hollyburn
    PRIVATE
        ${x86_specific_files}
)

target_sources(
    Silvertip
    PRIVATE
        ${x86_specific_files}
)

