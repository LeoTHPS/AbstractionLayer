#pragma once
#include "AL/Common.hpp"

namespace AL
{
	class NotImplementedException
		: public Exception
	{
	public:
		NotImplementedException()
			: Exception(
				"Not implemented"
			)
		{
		}
	};
}
