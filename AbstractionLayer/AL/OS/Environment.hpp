#pragma once
#include "AL/Common.hpp"

namespace AL::OS
{
	class Environment
	{
		Environment() = delete;

	public:
		// @throw AL::Exceptions::Exception
		// @return false if not found
		static bool TryGetVariable(const String& name, String& value)
		{
#if defined(AL_PLATFORM_LINUX)
			throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			DWORD bufferSize;

			if ((bufferSize = GetEnvironmentVariableA(name.GetCString(), nullptr, 0)) == 0)
			{
				if (GetLastError() == ERROR_ENVVAR_NOT_FOUND)
				{

					return false;
				}

				throw Exceptions::SystemException(
					"GetEnvironmentVariableA"
				);
			}

			value.SetCapacity(
				bufferSize
			);

			if ((bufferSize = GetEnvironmentVariableA(name.GetCString(), &value[0], bufferSize)) == 0)
			{
				if (GetLastError() == ERROR_ENVVAR_NOT_FOUND)
				{

					return false;
				}

				throw Exceptions::SystemException(
					"GetEnvironmentVariableA"
				);
			}

			value.RefreshLength();
#endif

			return true;
		}

		// @throw AL::Exceptions::Exception
		static void SetVariable(const String& name, const String& value)
		{
#if defined(AL_PLATFORM_LINUX)
			throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			if (SetEnvironmentVariableA(name.GetCString(), value.GetCString()) == 0)
			{

				throw Exceptions::SystemException(
					"SetEnvironmentVariableA"
				);
			}
#endif
		}
	};
}
