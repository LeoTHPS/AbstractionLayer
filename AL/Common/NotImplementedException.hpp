#pragma once
#include "AL/Common.hpp"

#define NotImplementedException() \
	_NotImplementedException(__FUNCTION__, __FILE__, __LINE__)

namespace AL
{
	class _NotImplementedException
		: public Exception
	{
		String function;
		String file;
		uint32 line;

	public:
		_NotImplementedException(String&& function, String&& file, uint32 line)
			: Exception(
				"Not implemented [Function: %s, File: %s, Line: %lu]",
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
