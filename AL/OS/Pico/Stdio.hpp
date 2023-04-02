#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_PICO)
	#error Platform not supported
#endif

#include <pico/stdio.h>

#if defined(LIB_PICO_STDIO_USB)
	#include <pico/stdio_usb.h>
#endif

#if defined(LIB_PICO_STDIO_UART)
	#include <pico/stdio_uart.h>
#endif

#if defined(LIB_PICO_STDIO_SEMIHOSTING)
	#include <pico/stdio_semihosting.h>
#endif

namespace AL::OS::Pico
{
	class Stdio
	{
		inline static Bool isInitialized = False;

		Stdio() = delete;

	public:
		static Bool IsInitialized()
		{
			return isInitialized;
		}

		static Bool IsUsbConnected()
		{
#if defined(LIB_PICO_STDIO_USB)
			if (::stdio_usb_connected())
			{

				return True;
			}
#endif

			return False;
		}

		static Void Init()
		{
			if (!IsInitialized())
			{
				::stdio_init_all();

				isInitialized = True;
			}
		}

		static Bool Flush()
		{
			if (!IsInitialized())
			{

				return False;
			}

			::stdio_flush();

			return True;
		}

		static Bool Filter(::stdio_driver& driver)
		{
			if (!IsInitialized())
			{

				return False;
			}

			::stdio_filter_driver(
				&driver
			);

			return True;
		}

		static Bool SetDriverEnabled(::stdio_driver& driver, Bool set = True)
		{
			if (!IsInitialized())
			{

				return False;
			}

			::stdio_set_driver_enabled(
				&driver,
				set
			);

			return True;
		}

		static Bool SetTranslateCRLF(::stdio_driver& driver, Bool set = True)
		{
			if (!IsInitialized())
			{

				return False;
			}

			::stdio_set_translate_crlf(
				&driver,
				set
			);

			return True;
		}
	};
}
