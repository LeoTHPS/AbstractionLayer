#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_LINUX)
	#error Platform not supported
#endif

#include "AL/OS/SystemException.hpp"

#include <cstdlib>

namespace AL::OS::Linux
{
	class Environment
	{
		Environment() = delete;

	public:
		// @throw AL::Exception
		static Bool Get(String& value, const String& name)
		{
			if (auto lpValue = ::getenv(name.GetCString()))
			{
				value.Assign(
					lpValue
				);

				return True;
			}

			return False;
		}

		// @throw AL::Exception
		static Void Set(const String& name, const String& value)
		{
			if (::setenv(name.GetCString(), value.GetCString(), 1) == -1)
			{

				throw SystemException(
					"setenv"
				);
			}
		}

		// @throw AL::Exception
		static Void Delete(const String& name)
		{
			if (::unsetenv(name.GetCString()) == -1)
			{

				throw SystemException(
					"unsetenv"
				);
			}
		}
	};
}
