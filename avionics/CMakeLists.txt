cmake_minimum_required(VERSION 3.10)
project(FLARE)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "-pthread -Wall -Wextra -pedantic")
else()
    set(CMAKE_CXX_FLAGS "-pthread -static-libstdc++ -static-libgcc -Wall -Wextra -pedantic")
endif()

# add_definitions()

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

###
### Google Test
###

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
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
