RM           ?= rm
CXX          ?= g++

SOURCE_FILES = main.cpp
OBJECT_FILES = $(SOURCE_FILES:.cpp=.o)

AL_INCLUDE   ?= ..

CPPFLAGS     += -DAL_TEST_SHOW_CONSOLE_OUTPUT
CXXFLAGS     += -Wall -Wfatal-errors -g -std=c++2a -I. -I"$(AL_INCLUDE)"
#CXXFLAGS     += -O3
#CXXFLAGS     += -Wno-unused-function

# Dependencies
## Linux:   libpthread-stubs0-dev libusb-1.0-0-dev libi2c-dev libgpiod-dev/wiringpi liblua5.4-dev
## Windows: 

ifdef COMPILER
	ifeq ($(COMPILER), GNU)
		CXX = g++
	endif

	ifeq ($(COMPILER), CLANG)
		CXX = clang++
	endif
endif

ifdef PLATFORM
	ifeq ($(PLATFORM), LINUX)
		LDLIBS += -lpthread -lusb-1.0 -li2c -lgpiod -llua
	endif

	ifeq ($(PLATFORM), WINDOWS)
		LDLIBS += -luser32 -lntdll -lkernel32
		LDLIBS += -lwsock32 -lws2_32
		LDLIBS += -lshlwapi
		LDLIBS += -lgdi32
		LDLIBS += -ld3d11 -ld2d1 -ldxguid -ldwrite -lwinmm -ldsound -lwindowscodecs -lOle32
	endif
endif

.PHONY: clean

Test: $(OBJECT_FILES)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS)

clean:
	$(RM) -f $(OBJECT_FILES)
