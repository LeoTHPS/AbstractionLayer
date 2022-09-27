#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_PICO)
	#warning Platform not supported
#endif

#if defined(AL_PLATFORM_PICO)
	#include "Pins.hpp"

	#include <hardware/gpio.h>
#endif

namespace AL::Hardware::Drivers::Pico
{
	class LED
	{
		inline static Bool isOpen = False;

		LED() = delete;

	public:
		static Bool IsOpen()
		{
			return isOpen;
		}

		// @throw AL::Exception
		static Void Open()
		{
			AL_ASSERT(
				!IsOpen(),
				"LED already open"
			);

#if defined(AL_PLATFORM_PICO)
			::gpio_init(
				PINS_LED
			);

			::gpio_set_dir(
				PINS_LED,
				true
			);

			::gpio_put(
				PINS_LED,
				false
			);
#else
			throw PlatformNotSupportedException();
#endif

			isOpen = True;
		}

		static Void Close()
		{
			if (IsOpen())
			{
#if defined(AL_PLATFORM_PICO)
				::gpio_deinit(
					PINS_LED
				);
#endif

				isOpen = False;
			}
		}

		// @throw AL::Exception
		static Void Read(Bool& value)
		{
			AL_ASSERT(
				IsOpen(),
				"LED not open"
			);

#if defined(AL_PLATFORM_PICO)
			value = ::gpio_get(PINS_LED);
#endif
		}

		// @throw AL::Exception
		static Void Write(Bool value)
		{
			AL_ASSERT(
				IsOpen(),
				"LED not open"
			);

#if defined(AL_PLATFORM_PICO)
			::gpio_put(
				PINS_LED,
				value
			);
#endif
		}
	};
}
