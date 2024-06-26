#pragma once
#include "AL/Common.hpp"

#if !AL_HAS_INCLUDE(<sqlite3.h>)
	#error Missing sqlite3.h
#endif

#include <sqlite3.h>

namespace AL::SQLite3
{
	class Exception
		: public AL::Exception
	{
		String function;
		int    errorCode;

	public:
		template<size_t S>
		Exception(::sqlite3* db, const char(&function)[S])
			: AL::Exception(
				"Error calling '%s': %s",
				&function[0],
				::sqlite3_errmsg(db)
			),
			function(
				function
			),
			errorCode(
				::sqlite3_errcode(db)
			)
		{
		}

		template<size_t S>
		Exception(::sqlite3* db, const char(&function)[S], int errorCode)
			: AL::Exception(
				"Error calling '%s': %s",
				&function[0],
				::sqlite3_errstr(errorCode)
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
