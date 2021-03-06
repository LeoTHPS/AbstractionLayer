#pragma once
#include "AL/Common.hpp"

#if defined(AL_PLATFORM_LINUX)
	#include <errno.h>
#endif

namespace AL::OS
{
#if defined(AL_PLATFORM_LINUX)
	typedef typename Get_Integer_Base<typename Remove_Modifiers<decltype(errno)>::Type>::Type ErrorCode;
#elif defined(AL_PLATFORM_WINDOWS)
	typedef typename Get_Integer_Base<typename Remove_Modifiers<decltype(::GetLastError())>::Type>::Type ErrorCode;
#endif

	inline ErrorCode GetLastError()
	{
		return static_cast<ErrorCode>(
#if defined(AL_PLATFORM_LINUX)
			errno
#elif defined(AL_PLATFORM_WINDOWS)
			::GetLastError()
#endif
		);
	}

	static String GetErrorString(ErrorCode errorCode)
	{
#if defined(AL_PLATFORM_LINUX)
		return String(
			strerror(static_cast<typename Remove_Modifiers<decltype(errno)>::Type>(errorCode))
		);
#elif defined(AL_PLATFORM_WINDOWS)
		auto string = String::Format(
			"%u",
			errorCode
		);

		LPVOID lpMessageBuffer;

		if (auto messageLength = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPSTR>(&lpMessageBuffer), 0, nullptr))
		{
			typename String::Container container(
				messageLength + 1
			);

			memcpy(
				&container[0],
				lpMessageBuffer,
				messageLength + 1
			);

			LocalFree(
				lpMessageBuffer
			);

			string = String(
				Move(container)
			);

			string.RemoveLast(
				"\n"
			);
			
			string.RemoveLast(
				"\r"
			);
		}

		return string;
#endif
	}

	inline String GetLastErrorString()
	{
		return GetErrorString(
			GetLastError()
		);
	}
}
