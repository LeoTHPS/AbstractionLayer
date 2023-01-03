#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_LINUX)
	#error Platform not supported
#endif

#include <dlfcn.h>

namespace AL::OS::Linux
{
	class DLException
		: public Exception
	{
	public:
		DLException()
			: Exception(
				GetLastError()
			)
		{
		}

		template<size_t S>
		explicit DLException(const char(&function)[S])
			: Exception(
				"Error calling '%s': %s",
				&function[0],
				GetLastError()
			)
		{
		}

	private:
		static const char* GetLastError()
		{
			auto message = ::dlerror();

			return (message != nullptr) ? message : "";
		}
	};
}
