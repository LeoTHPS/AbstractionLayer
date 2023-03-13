#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_RP2040)
	#error Platform not supported
#endif

#include <pico/bootrom.h>

namespace AL::Hardware::Pico
{
	class Boot
	{
		Boot() = delete;

	public:
		static Void RebootToMassStorage()
		{
			::reset_usb_boot(
				0,
				0
			);
		}
	};
}
