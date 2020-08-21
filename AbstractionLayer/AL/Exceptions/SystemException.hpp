#pragma once
#include "AL/Common.hpp"

#include "Exception.hpp"

#include "AL/OS/ErrorCode.hpp"

namespace AL::Exceptions
{
	class SystemException
		: public Exception
	{
		OS::ErrorCode errorCode;

	public:
		SystemException()
			: SystemException(
				OS::GetLastError()
			)
		{
		}

		explicit SystemException(OS::ErrorCode errorCode)
			: Exception(
				OS::GetErrorString(errorCode)
			)
		{
		}

		template<size_t S>
		explicit SystemException(const char(&function)[S])
			: SystemException(
				function,
				OS::GetLastError()
			)
		{
		}

		template<size_t S>
		SystemException(const char(&function)[S], OS::ErrorCode errorCode)
			: Exception(
				"Error calling '%s': %s",
				&function[0],
				OS::GetErrorString(errorCode).GetCString()
			)
		{
		}

		auto GetErrorCode() const
		{
			return errorCode;
		}
	};
}
