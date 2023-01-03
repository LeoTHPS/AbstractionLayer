#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_LINUX)
	#error Platform not supported
#endif

#include "AL/OS/Process.hpp"
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
		static Void Raise(int signal)
		{
			if (::raise(signal) != 0)
			{

				throw SystemException(
					"raise"
				);
			}
		}

		// @throw AL::Exception
		// @return AL::False if would block
		static Bool Post(ProcessId& processId, int signal)
		{
			if (::sigqueue(static_cast<::pid_t>(processId), signal, ::sigval { }) == -1)
			{
				auto errorCode = GetLastError();

				if (errorCode == EAGAIN)
				{

					return False;
				}

				throw SystemException(
					"sigqueue"
				);
			}

			return True;
		}

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
