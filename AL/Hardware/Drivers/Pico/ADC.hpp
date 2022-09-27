#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_PICO)
	#warning Platform not supported
#endif

#include "AL/Hardware/Drivers/RP2040/ADC.hpp"

namespace AL::Hardware::Drivers::Pico
{
	typedef AL::Hardware::Drivers::RP2040::ADC         ADC;
	typedef AL::Hardware::Drivers::RP2040::ADCChannels ADCChannels;
}
