RM           ?= rm -f
CXX          ?= g++

SOURCE_FILES = main.cpp
OBJECT_FILES = $(SOURCE_FILES:.cpp=.o)

AL_INCLUDE   ?= ..

#CPPFLAGS     += -DNDEBUG
CPPFLAGS     += -DAL_TEST_SHOW_CONSOLE_OUTPUT
CXXFLAGS     += -Wall -Wfatal-errors -g -O0 -std=c++20 -I. -I"$(AL_INCLUDE)"
#CXXFLAGS     += -Wno-unused-function -Wno-unused-variable -Wno-unused-lambda-capture -Wno-unused-private-field
#CXXFLAGS     += -Wno-format-security

ifdef COMPILER
	ifeq ($(COMPILER), GNU)
		CXX = g++
	else ifeq ($(COMPILER), CLANG)
		CXX = clang++
	else
		CXX = $(COMPILER)
	endif
endif

ifdef PLATFORM
	ifeq ($(PLATFORM), LINUX)
#		libusb-1.0-0-dev libi2c-dev libgpiod-dev libgl-dev libglu1-mesa-dev liblua5.4-dev libsqlite3-dev libssl-dev

		LDLIBS += -ldl -lpthread -lusb-1.0 -li2c -lgpiod
		LDLIBS += -lGL -lGLU
		LDLIBS += -llua5.4
		LDLIBS += -lssl -lcrypto
		LDLIBS += -lsqlite3
	endif

	ifeq ($(PLATFORM), WINDOWS)
		LDLIBS += -luser32 -lntdll -lkernel32 -lAdvapi32
		LDLIBS += -lwsock32 -lws2_32 -lIphlpapi
		LDLIBS += -lshlwapi
		LDLIBS += -lgdi32
		LDLIBS += -ld3d11 -ld2d1 -ldxguid -ldwrite -lwinmm -ldsound -lwindowscodecs -lOle32
		LDLIBS += -lopengl32 -lglu32
		LDLIBS += -llua
		LDLIBS += -lssl -lcrypto
		LDLIBS += -lsqlite3
	endif
endif

.PHONY: clean

Test: $(OBJECT_FILES)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS)

clean:
	$(RM) $(OBJECT_FILES)
