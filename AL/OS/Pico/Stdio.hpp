#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_PICO)
	#error Platform not supported
#endif

#include <pico/stdio.h>

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

		static Void Init()
		{
			if (!IsInitialized())
			{
				::stdio_init_all();

				isInitialized = True;
			}
		}
	};
}
