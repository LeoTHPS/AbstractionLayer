#pragma once
#include "AL/Common.hpp"

namespace AL
{
	class PlatformNotSupportedException
		: public Exception
	{
	public:
		PlatformNotSupportedException()
			: Exception(
				"Platform not supported"
			)
		{
		}
	};
}
