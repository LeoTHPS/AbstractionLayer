CP             ?= cp -f
RM             ?= rm -f
CC             ?= gcc
CXX            ?= g++
#CC             = arm-linux-gnueabihf-gcc
#CXX            = arm-linux-gnueabihf-g++

OUT_FILE       = AbstractionLayer.Lua.so
OUT_DIRECTORY  = ../Build
OUT_FILE_PATH  = $(OUT_DIRECTORY)/$(OUT_FILE)

SOURCE_FILES   = $(wildcard Lua543/*.c)
OBJECT_FILES   = $(SOURCE_FILES:.c=.o)

AL_INCLUDE     = ../AbstractionLayer

CFLAGS         += -fPIC -O3
CXXFLAGS       += -fPIC -Wall -O3 -std=c++17 -I$(AL_INCLUDE) -I.
LDFLAGS        += -shared

.PHONY: all clean

all: $(OBJECT_FILES)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $(OUT_FILE_PATH) $(LDFLAGS) $(LDLIBS)

clean:
	$(RM) $(OBJECT_FILES)

install:
	$(CP) $(OUT_FILE_PATH) /usr/lib/lib$(OUT_FILE)

uninstall:
	$(RM) /usr/lib/lib$(OUT_FILE)
