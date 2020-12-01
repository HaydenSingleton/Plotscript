ifeq ($(OS),Windows_NT) 
    detected_OS := Windows
else
    detected_OS := $(shell sh -c 'uname 2>/dev/null || echo Unknown')
endif

test:
	$(info $(detected_OS))

CXX=g++
PROJECT=PLOTSCRIPT
STARTUP_FILE=startup.pls

ifeq ($(detected_OS),Windows)
    all: LINX
else
    all: WIND
endif

WIND: TUI_MAIN

LINX:

TOKEN=token.hpp token.cpp
ATOM=atom.hpp atom.cpp
ENVIORMENT=enviorment.cpp enviorment.hpp
PARSE=parse.hpp parse.cpp
INTERPRETER=interpreter.hpp interpreter.cpp

INTERPRETER_SRC=$(TOKEN) $(ATOM) $(ENVIORMENT)\
 $(PARSE) $(INTERPRETER) TSmessage.hpp

UNITTEST_SRC=catch.hpp atom_tests.cpp \
 environment_tests.cpp expression_tests.cpp\
 interpreter_tests.cpp parse_tests.cpp\
 semantic_error.hpp token_tests.cpp\
 unit_tests.cpp TSmessage_tests.cpp;\

GUI_SRC=notebook_test.cpp

TUI_MAIN: plotscript.cpp
    $(CXX) -o plotscript $(INTERPRETER_SRC)
GUI_MAIN=notebook.cpp

