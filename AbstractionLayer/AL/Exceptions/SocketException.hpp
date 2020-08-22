#pragma once
#include "AL/Common.hpp"

#include "SystemException.hpp"

namespace AL::Exceptions
{
	class SocketException
		: public SystemException
	{
	public:
		using SystemException::SystemException;
	};
}
