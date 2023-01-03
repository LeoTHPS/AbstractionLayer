#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_PICO_W)
	#warning Platform not supported
#endif

#include "AL/Hardware/Drivers/Pico/GPIO.hpp"

namespace AL::Hardware::Drivers::PicoW
{
	typedef Hardware::Drivers::Pico::GPIO              GPIO;
	typedef Hardware::Drivers::Pico::GPIOPin           GPIOPin;
	typedef Hardware::Drivers::Pico::GPIOPinEdges      GPIOPinEdges;
	typedef Hardware::Drivers::Pico::GPIOPinValues     GPIOPinValues;
	typedef Hardware::Drivers::Pico::GPIOPinDirections GPIOPinDirections;
}
