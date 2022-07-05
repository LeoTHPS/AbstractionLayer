#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_PICO_W)
	#warning Platform not supported
#endif

#include "AL/Hardware/RP2040/ADC.hpp"

namespace AL::Hardware::PicoW
{
	typedef AL::Hardware::RP2040::ADC         ADC;
	typedef AL::Hardware::RP2040::ADCChannels ADCChannels;
}
