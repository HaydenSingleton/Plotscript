# CMakeList.txt : CMake project for interpreter, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.12)

# build interpreter library
set(interpreter_files
	token.h token.cpp
	atom.h atom.cpp
	environment.h environment.cpp
	expression.h expression.cpp
	parse.h parse.cpp
	interpreter.h interpreter.cpp
	interrupt_handler.h
)
add_library(interpreter ${interpreter_files})

# Add source to this project's executable.
add_executable (plotscript "main.cpp")
target_link_libraries(plotscript interpreter)


install(TARGETS plotscript RUNTIME DESTINATION bin)
