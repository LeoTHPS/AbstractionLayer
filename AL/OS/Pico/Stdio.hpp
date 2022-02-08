#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_PICO)
	#error Platform not supported
#endif

#include "AL/OS/SystemException.hpp"

namespace AL::OS::Pico
{
	class Stdio
	{
		Stdio() = delete;

	public:
		static Void InitAll()
		{
			// TODO: implement
			throw NotImplementedException();
		}
	};
}
