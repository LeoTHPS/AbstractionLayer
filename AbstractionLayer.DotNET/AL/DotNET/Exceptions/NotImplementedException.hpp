#pragma once
#include "AL/DotNET/Common.hpp"

namespace AL::DotNET::Exceptions
{
	public ref class NotImplementedException
		: public Exception
	{
	public:
		NotImplementedException()
			: Exception(
				AL::Exceptions::NotImplementedException()
			)
		{
		}
	};
}
