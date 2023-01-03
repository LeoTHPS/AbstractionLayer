#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_PICO)
	#error Platform not supported
#endif

#include "Pins.hpp"

#include <hardware/gpio.h>

namespace AL::Hardware::Pico
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

			isOpen = True;
		}

		static Void Close()
		{
			if (IsOpen())
			{
				::gpio_deinit(
					PINS_LED
				);

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

			value = ::gpio_get(PINS_LED);
		}

		// @throw AL::Exception
		static Void Write(Bool value)
		{
			AL_ASSERT(
				IsOpen(),
				"LED not open"
			);

			::gpio_put(
				PINS_LED,
				value
			);
		}
	};
}
