#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_PICO_W)
	#warning Platform not supported
#endif

#include "AL/Hardware/Drivers/Pico/I2C.hpp"

namespace AL::Hardware::Drivers::PicoW
{
	typedef Hardware::Drivers::Pico::I2C        I2C;
	typedef Hardware::Drivers::Pico::I2CAddress I2CAddress;
}
