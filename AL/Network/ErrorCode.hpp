#pragma once
#include "AL/Common.hpp"

#include "AL/OS/ErrorCode.hpp"

namespace AL::Network
{
	typedef OS::ErrorCode ErrorCode;

	inline ErrorCode GetLastError()
	{
		return OS::GetLastError();
	}

	inline String GetErrorString(ErrorCode errorCode)
	{
		return OS::GetErrorString(
			errorCode
		);
	}

	inline String GetLastErrorString()
	{
		return OS::GetLastErrorString();
	}
}
