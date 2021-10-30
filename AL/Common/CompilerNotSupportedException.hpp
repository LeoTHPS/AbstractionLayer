#pragma once
#include "AL/Common.hpp"

namespace AL
{
	class CompilerNotSupportedException
		: public Exception
	{
	public:
		CompilerNotSupportedException()
			: Exception(
				"Compiler not supported"
			)
		{
		}
	};
}
