#pragma once
#include "AL/Common.hpp"

#include "ErrorCode.hpp"

namespace AL::OS
{
	class SystemException
		: public Exception
	{
		ErrorCode errorCode;

	public:
		SystemException()
			: SystemException(
				GetLastError()
			)
		{
		}

		explicit SystemException(ErrorCode errorCode)
			: Exception(
				GetErrorString(
					errorCode
				)
			)
		{
		}

		template<size_t S>
		explicit SystemException(const char(&function)[S])
			: SystemException(
				function,
				GetLastError()
			)
		{
		}

		template<size_t S>
		SystemException(const char(&function)[S], ErrorCode errorCode)
			: Exception(
				"Error calling '%s': %s",
				&function[0],
				GetErrorString(errorCode).GetCString()
			)
		{
		}

		auto GetErrorCode() const
		{
			return errorCode;
		}
	};
}
