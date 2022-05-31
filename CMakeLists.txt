# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.12)
set(CMAKE_CXX_STANDARD 17)

project ("ProjectScript" VERSION 1.3 HOMEPAGE_URL "https://github.com/HaydenSingleton/Plotscript")

# Include sub-projects.
add_subdirectory ("app")
add_subdirectory ("tests")

# Generate and include start up plotscript lambdas
set(STARTUP_FILE app/startup.script)
configure_file(
	${CMAKE_SOURCE_DIR}/app/startup_config.h.in ${CMAKE_BINARY_DIR}/startup_config.h
)
include_directories(${CMAKE_BINARY_DIR})