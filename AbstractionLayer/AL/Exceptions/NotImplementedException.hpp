#pragma once
#include "AL/Common.hpp"

#include "Exception.hpp"

namespace AL::Exceptions
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
