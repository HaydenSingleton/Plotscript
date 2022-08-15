# CMakeList.txt : CMake project for tests
cmake_minimum_required (VERSION 3.12)

set(test_src doctest.h test_main.cpp test_atom.cpp test_environment.cpp test_expression.cpp test_interpreter.cpp test_parse.cpp test_token.cpp validation_tests.cpp)

# Add source to this project's executable.
add_executable (tests ${test_src})
include_directories(${CMAKE_SOURCE_DIR}/Main/src ${CMAKE_SOURCE_DIR}/Main/inc)
target_link_libraries(tests interpreter)

install(TARGETS tests RUNTIME DESTINATION bin)
