#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_LINUX)
	#error Platform not supported
#endif

#include "AL/OS/SystemException.hpp"

#include <cstdlib>

namespace AL::OS::Linux
{
	// @throw AL::Exception
	// @return AL::False to stop
	typedef Function<Bool(const String& name, const String& value)> EnvironmentOnEnumCallback;

	class Environment
	{
		Environment() = delete;

	public:
		// @throw AL::Exception
		// @return AL::False to stop
		typedef EnvironmentOnEnumCallback OnEnumCallback;

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

		// @throw AL::Exception
		static Void Enumerate(const OnEnumCallback& callback)
		{
			if (auto lpEnvironmentStrings = ::environ)
			{
				for (auto lpEnvironmentString = *lpEnvironmentStrings; lpEnvironmentString != nullptr; lpEnvironmentString = *++lpEnvironmentStrings)
				{
					String environmentString(lpEnvironmentString);
					auto   environmentStringSplitOffset = environmentString.IndexOf('=');
					auto   environmentName              = environmentString.SubString(0, environmentStringSplitOffset);
					auto   environmentValue             = environmentString.SubString(environmentStringSplitOffset + 1);

					if (!callback(environmentName, environmentValue))
					{

						break;
					}
				}
			}
		}
	};
}
