#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_PICO)
	#error Platform not supported
#endif

#include "AL/Hardware/Drivers/RP2040/I2C.hpp"

namespace AL::Hardware::Drivers::Pico
{
	typedef Drivers::RP2040::I2C        I2C;
	typedef Drivers::RP2040::I2CAddress I2CAddress;
}
