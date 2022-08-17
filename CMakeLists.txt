# CMakeList.txt : Top-level CMake project file, do global configuration
# and inc sub-projects here.
#
cmake_minimum_required (VERSION 3.12)
set(CMAKE_CXX_STANDARD 23)
project ("Plotscript" VERSION 1.4)

# Include sub-projects.
add_subdirectory ("PlotscriptApp")
add_subdirectory ("Tests")