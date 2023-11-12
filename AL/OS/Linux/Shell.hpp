#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_LINUX)
	#error Platform not supported
#endif

#include "AL/OS/SystemException.hpp"

#include <stdlib.h>

namespace AL::OS::Linux
{
	class Shell
	{
		Shell() = delete;

	public:
		// @throw AL::Exception
		static Void Execute(const String& value)
		{
			if (::system(value.GetCString()) == -1)
			{

				throw SystemException(
					"system"
				);
			}
		}
	};
}
