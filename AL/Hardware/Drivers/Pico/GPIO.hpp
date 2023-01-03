#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_PICO)
	#error Platform not supported
#endif

#include "AL/Hardware/Drivers/RP2040/GPIO.hpp"

namespace AL::Hardware::Drivers::Pico
{
	typedef Drivers::RP2040::GPIO              GPIO;
	typedef Drivers::RP2040::GPIOPin           GPIOPin;
	typedef Drivers::RP2040::GPIOPinEdges      GPIOPinEdges;
	typedef Drivers::RP2040::GPIOPinValues     GPIOPinValues;
	typedef Drivers::RP2040::GPIOPinDirections GPIOPinDirections;
}
