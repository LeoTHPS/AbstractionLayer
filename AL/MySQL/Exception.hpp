#pragma once
#include "AL/Common.hpp"

#if !AL_HAS_INCLUDE(<mysql.h>)
	#error Missing mysql.h
#endif

#include <mysql.h>

namespace AL::MySQL
{
	class Exception
		: public AL::Exception
	{
		String function;
		int    errorCode;

	public:
		template<size_t S>
		Exception(const char(&function)[S])
			: AL::Exception(
				"Error calling '%s'",
				&function[0]
			),
			function(
				function
			),
			errorCode(
				0
			)
		{
		}
		template<size_t S>
		Exception(::MYSQL* db, const char(&function)[S])
			: AL::Exception(
				"Error calling '%s': %s",
				&function[0],
				::mysql_error(db)
			),
			function(
				function
			),
			errorCode(
				::mysql_errno(db)
			)
		{
		}
		template<size_t S>
		Exception(const char(&function)[S], int errorCode, const String& errorString)
			: AL::Exception(
				"Error calling '%s': %s",
				&function[0],
				errorString.GetCString()
			),
			function(
				function
			),
			errorCode(
				errorCode
			)
		{
		}

		auto& GetFunction() const
		{
			return function;
		}

		auto GetErrorCode() const
		{
			return errorCode;
		}
	};
}
