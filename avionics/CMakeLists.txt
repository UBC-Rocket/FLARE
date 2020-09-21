cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(CMAKE_HOST_APPLE)
    set(CMAKE_CXX_FLAGS "-pthread -static-libstdc++ -Wall -Wextra -pedantic")
else()
    set(CMAKE_CXX_FLAGS "-pthread -static-libstdc++ -static-libgcc -Wall -Wextra -pedantic")
endif()


project(Avionics_Native)
add_executable(TantalusStage1 common/src/avionics.cpp)

add_compile_definitions(RADIO_CONFIG_PACKET_SIM_ACTIVE=1)

add_subdirectory(common)
add_subdirectory(envs/x86)
add_subdirectory(rockets/tantalus_stage_1)

target_include_directories(TantalusStage1 PUBLIC
    common/include
    common/lib
    envs/x86/include
    envs/x86/lib
    rockets/tantalus_stage_1/include
)
target_compile_definitions(TantalusStage1 PUBLIC
    RADIO_CONFIG_PACKET_ROCKET_ID=0
)
