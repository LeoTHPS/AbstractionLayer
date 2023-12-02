#pragma once
#include "AL/Common.hpp"

#if defined(AL_PLATFORM_LINUX)
	#include "Linux/Environment.hpp"
#elif defined(AL_PLATFORM_WINDOWS)
	#include "Windows/Environment.hpp"
#else
	#error Platform not supported
#endif

namespace AL::OS
{
#if defined(AL_PLATFORM_LINUX)
	// @throw AL::Exception
	// @return AL::False to stop
	typedef Linux::EnvironmentOnEnumCallback   EnvironmentOnEnumCallback;
#elif defined(AL_PLATFORM_WINDOWS)
	// @throw AL::Exception
	// @return AL::False to stop
	typedef Windows::EnvironmentOnEnumCallback EnvironmentOnEnumCallback;
#endif

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
#if defined(AL_PLATFORM_LINUX)
			if (!Linux::Environment::Get(value, name))
			{

				return False;
			}
#elif defined(AL_PLATFORM_WINDOWS)
			if (!Windows::Environment::Get(value, name))
			{

				return False;
			}
#endif

			return True;
		}

		// @throw AL::Exception
		static Void Set(const String& name, const String& value)
		{
#if defined(AL_PLATFORM_LINUX)
			Linux::Environment::Set(
				name,
				value
			);
#elif defined(AL_PLATFORM_WINDOWS)
			Windows::Environment::Set(
				name,
				value
			);
#endif
		}

		// @throw AL::Exception
		static Void Delete(const String& name)
		{
#if defined(AL_PLATFORM_LINUX)
			Linux::Environment::Delete(
				name
			);
#elif defined(AL_PLATFORM_WINDOWS)
			Windows::Environment::Delete(
				name
			);
#endif
		}

		// @throw AL::Exception
		static Void Enumerate(const OnEnumCallback& callback)
		{
#if defined(AL_PLATFORM_LINUX)
			Linux::Environment::Enumerate(
				callback
			);
#elif defined(AL_PLATFORM_WINDOWS)
			Windows::Environment::Enumerate(
				callback
			);
#endif
		}
	};
}
