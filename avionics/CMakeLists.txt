cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "-pthread -static-libstdc++ -static-libgcc")
add_definitions(-Wall)

project(Avionics_Native)


add_executable(TantalusStage1 common/src/avionics.cpp)

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

