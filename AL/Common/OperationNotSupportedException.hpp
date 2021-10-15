#pragma once
#include "AL/Common.hpp"

namespace AL
{
	class OperationNotSupportedException
		: public Exception
	{
	public:
		OperationNotSupportedException()
			: Exception(
				"Operation not supported"
			)
		{
		}
	};
}
