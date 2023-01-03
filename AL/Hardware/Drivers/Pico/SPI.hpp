#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_PICO)
	#error Platform not supported
#endif

#include "AL/Hardware/Drivers/RP2040/SPI.hpp"

namespace AL::Hardware::Drivers::Pico
{
	typedef Drivers::RP2040::SPI      SPI;
	typedef Drivers::RP2040::SPIModes SPIModes;
}
