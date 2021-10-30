#pragma once
#include "AL/Common.hpp"

#if defined(AL_PLATFORM_LINUX)
	#include <errno.h>
#endif

namespace AL::OS
{
#if defined(AL_PLATFORM_LINUX)
	typedef typename Get_Enum_Or_Integer_Base<typename Remove_Modifiers<decltype(errno)>::Type>::Type            ErrorCode;
#elif defined(AL_PLATFORM_WINDOWS)
	typedef typename Get_Enum_Or_Integer_Base<typename Remove_Modifiers<decltype(::GetLastError())>::Type>::Type ErrorCode;
#endif

	inline ErrorCode GetLastError()
	{
#if defined(AL_PLATFORM_LINUX)
		return static_cast<ErrorCode>(
			errno
		);		
#elif defined(AL_PLATFORM_WINDOWS)
		return static_cast<ErrorCode>(
			::GetLastError()
		);
#else
		throw PlatformNotSupportedException();
#endif
	}

	inline String GetErrorString(ErrorCode errorCode)
	{
#if defined(AL_PLATFORM_LINUX)
		typedef typename Remove_Modifiers<decltype(errno)>::Type Base;

		auto lpString = ::strerror(
			static_cast<Base>(errorCode)
		);

		return String(
			lpString
		);
#elif defined(AL_PLATFORM_WINDOWS)
		auto string = String::Format(
			"%u",
			errorCode
		);

		LPVOID lpMessageBuffer;

		if (auto messageLength = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPSTR>(&lpMessageBuffer), 0, nullptr))
		{
			string.Assign(
				reinterpret_cast<const typename String::Char*>(lpMessageBuffer),
				messageLength
			);

			LocalFree(
				lpMessageBuffer
			);

			if (string.EndsWith("\r\n"))
			{

				string.RemoveLast(
					"\r\n"
				);
			}
		}

		return string;
#else
		throw PlatformNotSupportedException();
#endif
	}

	inline String GetLastErrorString()
	{
		return GetErrorString(
			GetLastError()
		);
	}
}
