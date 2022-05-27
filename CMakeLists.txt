# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("ProjectScript")

set (SOURCEDIR "app")
set (TESTDIR "tests")

# Include sub-projects.
add_subdirectory (${SOURCEDIR})
add_subdirectory (${TESTDIR})

set(STARTUP_FILE ${SOURCEDIR}/startup.script)
configure_file(${SOURCEDIR}/startup_config.h.in ${CMAKE_BINARY_DIR}/startup_config.h)
include_directories(${CMAKE_BINARY_DIR})