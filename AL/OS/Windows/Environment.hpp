#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_WINDOWS)
	#error Platform not supported
#endif

#include "AL/OS/SystemException.hpp"

namespace AL::OS::Windows
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
			::DWORD bufferSize;

			if ((bufferSize = ::GetEnvironmentVariableA(name.GetCString(), nullptr, 0)) == 0)
			{
				auto errorCode = GetLastError();

				if (errorCode == ERROR_ENVVAR_NOT_FOUND)
				{

					return False;
				}

				throw SystemException(
					"GetEnvironmentVariableA",
					errorCode
				);
			}

			value.SetCapacity(
				bufferSize - 1
			);

			if ((bufferSize = ::GetEnvironmentVariableA(name.GetCString(), &value[0], bufferSize)) == 0)
			{
				auto errorCode = GetLastError();

				if (errorCode == ERROR_ENVVAR_NOT_FOUND)
				{

					return False;
				}

				throw SystemException(
					"GetEnvironmentVariableA",
					errorCode
				);
			}

			return True;
		}

		// @throw AL::Exception
		static Void Set(const String& name, const String& value)
		{
			if (!::SetEnvironmentVariableA(name.GetCString(), value.GetCString()))
			{

				throw SystemException(
					"SetEnvironmentVariableA"
				);
			}
		}

		// @throw AL::Exception
		static Void Delete(const String& name)
		{
			if (!::SetEnvironmentVariableA(name.GetCString(), nullptr))
			{

				throw SystemException(
					"SetEnvironmentVariableA"
				);
			}
		}

		// @throw AL::Exception
		static Void Enumerate(const OnEnumCallback& callback)
		{
			::LPCH lpEnvironmentStrings;

			if ((lpEnvironmentStrings = ::GetEnvironmentStrings()) == NULL)
			{

				throw SystemException(
					"GetEnvironmentStrings"
				);
			}

			for (size_t environmentStringLength = 0; (environmentStringLength = String::GetLength(lpEnvironmentStrings, environmentStringLength)) != 0; lpEnvironmentStrings += (environmentStringLength + 1))
			{
				String environmentString(lpEnvironmentStrings, environmentStringLength);
				auto   environmentStringSplitOffset = environmentString.IndexOf('=');

				if (environmentStringSplitOffset == String::NPOS)
					continue;

				auto environmentName  = environmentString.SubString(0, environmentStringSplitOffset);
				auto environmentValue = environmentString.SubString(environmentStringSplitOffset + 1);

				if (!callback(environmentName, environmentValue))
				{

					break;
				}
			}
		}
	};
}
