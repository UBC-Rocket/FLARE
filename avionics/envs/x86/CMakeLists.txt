# file(GLOB x86_files ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
# file(GLOB x86_HAL ${CMAKE_CURRENT_LIST_DIR}/lib/HAL/*.cpp)

target_sources( 
    myLib
    PUBLIC
        # ${x86_files}
        # ${x86_HAL}
        ${CMAKE_CURRENT_LIST_DIR}/lib/HAL/pin_util.cpp
        ${CMAKE_CURRENT_LIST_DIR}/lib/HAL/time.cpp
        ${CMAKE_CURRENT_LIST_DIR}/lib/HAL/port_impl.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/stdio_controller.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/env_config.cpp
)
