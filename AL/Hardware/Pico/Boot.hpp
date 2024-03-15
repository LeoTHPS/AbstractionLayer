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
		static Void Reboot()
		{
			*reinterpret_cast<volatile uint32*>(PPB_BASE + 0xED0C) = 0x05FA0004;
		}

		static Void RebootToMassStorage()
		{
			::reset_usb_boot(
				0,
				0
			);
		}
	};
}
