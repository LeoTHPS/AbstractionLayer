RM           ?= rm -f
CXX          ?= g++

SOURCE_FILES = main.cpp
OBJECT_FILES = $(SOURCE_FILES:.cpp=.o)

AL_INCLUDE   ?= ..

CPPFLAGS     += -D_DEBUG -DAL_TEST_SHOW_CONSOLE_OUTPUT
CXXFLAGS     += -Wall -Wfatal-errors -g -std=c++2a -I. -I"$(AL_INCLUDE)"
#CXXFLAGS     += -Wno-unused-function -Wno-unused-variable -Wno-unused-lambda-capture -Wno-unused-private-field
#CXXFLAGS     += -Wno-format-security

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
		LDLIBS += -ldl -lpthread -lusb-1.0 -li2c -lgpiod
		LDLIBS += -lGL -lGLU
		LDLIBS += -llua
		LDLIBS += -lsqlite3
	endif

	ifeq ($(PLATFORM), WINDOWS)
		LDLIBS += -luser32 -lntdll -lkernel32
		LDLIBS += -lwsock32 -lws2_32 -lIphlpapi
		LDLIBS += -lshlwapi
		LDLIBS += -lgdi32
		LDLIBS += -ld3d11 -ld2d1 -ldxguid -ldwrite -lwinmm -ldsound -lwindowscodecs -lOle32
		LDLIBS += -lopengl32 -lglu32
		LDLIBS += -llua
		LDLIBS += -lsqlite3
	endif
endif

.PHONY: clean

Test: $(OBJECT_FILES)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS)

clean:
	$(RM) $(OBJECT_FILES)
