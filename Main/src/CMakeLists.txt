# CMakeList.txt : CMake project for interpreter, inc source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.12)

# build interpreter library
set(interpreter_src
	token.cpp
	atom.cpp
	environment.cpp
	expression.cpp
	parse.cpp
	interpreter.cpp
)
include_directories(${CMAKE_SOURCE_DIR}/Main/inc)
add_library(interpreter ${interpreter_src})

# Add source to this project's executable.
add_executable (plotscript "main.cpp")
target_link_libraries(plotscript interpreter)
install(TARGETS plotscript RUNTIME DESTINATION bin)

# Generate and inc start up plotscript lambdas
set(STARTUP_FILE ${CMAKE_SOURCE_DIR}/Main/inc/startup.script)
configure_file(
	${CMAKE_SOURCE_DIR}/Main/inc/startup_config.h.in
	${CMAKE_SOURCE_DIR}/Main/inc/startup_config.h
)