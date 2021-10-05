#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_LINUX)
	#error Platform not supported
#endif

#include "AL/OS/SystemException.hpp"

#include <signal.h>

namespace AL::OS::Linux
{
	typedef Void(SignalHandler)(int signal);

	class Signal
	{
		Signal() = delete;

	public:
		// @throw AL::Exception
		// @return previous handler
		static SignalHandler* SetHandler(int signal, SignalHandler* lpHandler)
		{
			if ((lpHandler = ::signal(signal, lpHandler)) == SIG_ERR)
			{

				throw SystemException(
					"signal"
				);
			}

			return lpHandler;
		}
	};
}
