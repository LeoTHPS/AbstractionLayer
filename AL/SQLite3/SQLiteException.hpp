#pragma once
#include "AL/Common.hpp"

#if AL_HAS_INCLUDE(<sqlite3.h>)
	#define AL_DEPENDENCY_SQLITE3

	#include <sqlite3.h>
#else
	typedef void* sqlite3;

	constexpr int sqlite3_errcode(sqlite3*)
	{
		return -1;
	}

	constexpr const char* sqlite3_errmsg(sqlite3*)
	{
		return "Unknown";
	}

	constexpr const char* sqlite3_errstr(int)
	{
		return "Unknown";
	}
#endif

namespace AL::SQLite3
{
	class SQLiteException
		: public Exception
	{
		String function;
		int    errorCode;

	public:
		template<size_t S>
		SQLiteException(::sqlite3* db, const char(&function)[S])
			: Exception(
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
		SQLiteException(::sqlite3* db, const char(&function)[S], int errorCode)
			: Exception(
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
