#pragma once
#include "AL/Common.hpp"

#define OperationNotSupportedException() \
	_OperationNotSupportedException(__FUNCTION__, __FILE__, __LINE__)

namespace AL
{
	class _OperationNotSupportedException
		: public Exception
	{
		String function;
		String file;
		uint32 line;

	public:
		_OperationNotSupportedException(String&& function, String&& file, uint32 line)
			: Exception(
				"Operation not supported [Function: %s, File: %s, Line: %lu]",
				function.GetCString(),
				file.GetCString(),
				line
			),
			function(
				Move(function)
			),
			file(
				Move(file)
			),
			line(
				line
			)
		{
		}

		auto GetLine() const
		{
			return line;
		}

		auto& GetFile() const
		{
			return file;
		}

		auto& GetFunction() const
		{
			return function;
		}
	};
}
