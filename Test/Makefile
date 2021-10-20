RM           ?= rm
CXX          ?= g++

SOURCE_FILES = main.cpp
OBJECT_FILES = $(SOURCE_FILES:.cpp=.o)

AL_INCLUDE   ?= ..

CPPFLAGS     += -DAL_TEST_SHOW_CONSOLE_OUTPUT
CXXFLAGS     += -Wall -g -std=c++2a -I"$(AL_INCLUDE)"
#CXXFLAGS     += -O3
#CXXFLAGS     += -Wno-unused-function
LDLIBS       += -lpthread -lusb-1.0 -li2c -lgpiod -llua

# Dependencies: apt install libpthread-stubs0-dev libusb-1.0-0-dev libi2c-dev libgpiod-dev/wiringpi liblua

.PHONY: clean

Test: $(OBJECT_FILES)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS)

clean:
	$(RM) -f $(OBJECT_FILES)
