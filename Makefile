ifeq ($(OS),Windows_NT) 
    detected_OS := Windows
else
    detected_OS := $(shell sh -c 'uname 2>/dev/null || echo Unknown')
endif

test:
	$(info $(detected_OS))
