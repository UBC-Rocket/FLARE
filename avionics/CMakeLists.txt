cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
project(FLARE)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "-pthread -Wall -Wextra -pedantic -g")
else()
    set(CMAKE_CXX_FLAGS "-pthread -static-libstdc++ -static-libgcc -Wall -Wextra -pedantic -g")
endif()

add_executable(TantalusStage1 common/src/avionics.cpp)
add_executable(TantalusStage2 common/src/avionics.cpp)
add_executable(Hollyburn common/src/avionics.cpp)

add_subdirectory(common)
add_subdirectory(envs/x86)
add_subdirectory(rockets/tantalus_stage_1)
add_subdirectory(rockets/tantalus_stage_2)
add_subdirectory(rockets/hollyburn)

include_directories(common/include
common/lib
envs/x86/include
envs/x86/lib
)

# Rocket-specific directories
target_include_directories(TantalusStage1 PUBLIC
  rockets/tantalus_stage_1/include
)

target_include_directories(TantalusStage2 PUBLIC
rockets/tantalus_stage_2/include
)

target_include_directories(Hollyburn PUBLIC
rockets/hollyburn/include
)

###
### Setup Radio defines
###

add_compile_definitions(RADIO_CONFIG_PACKET_SIM_ACTIVE=1)
target_compile_definitions(TantalusStage1 PUBLIC
    RADIO_CONFIG_PACKET_ROCKET_ID=0
)
target_compile_definitions(TantalusStage2 PUBLIC
    RADIO_CONFIG_PACKET_ROCKET_ID=1
)

target_compile_definitions(Hollyburn PUBLIC
    RADIO_CONFIG_PACKET_ROCKET_ID=3
)

###
### Setup Git version
###
message(STATUS "Resolving GIT Version")
message("Directory: ${CMAKE_CURRENT_LIST_DIR}")
set(_git_build_version "unknown")

find_package(Git)
if(GIT_FOUND)
execute_process(
  COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
  WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
  OUTPUT_VARIABLE _git_build_version
  ERROR_QUIET
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  message( STATUS "GIT hash: ${_git_build_version}")
else()
  message(SEND_ERROR "Git not found; unable to set Radio Config data")
endif()
add_compile_definitions(RADIO_CONFIG_PACKET_VERSION_STR="${_git_build_version}")


###
### Google Test
###

# Download and unpack googletest at configure time
# -Wno-dev suppresses warnings - CMake gets confused when you do this, I guess
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -Wno-dev -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)
# Note 2020-12-20 - above add_subdirectory call throws CMake versioning errors.
# There's a recent (as of writing) pull request in googletest upstream
# https://github.com/google/googletest/pull/3094

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include")
endif()

enable_testing()

###
### Link all tests here
###
### Note: Compile project in build folder to keep unnecessarily files out from main directory
###

# Now simply link against gtest or gtest_main as needed. Eg
add_executable(Test tests/SampleTest.cpp)
target_link_libraries(Test gtest_main)
add_test(NAME SampleTest COMMAND Test)

# We can now call make test to run all the tests
