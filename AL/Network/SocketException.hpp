#pragma once
#include "AL/Common.hpp"

#include "ErrorCode.hpp"

#include "AL/OS/SystemException.hpp"

namespace AL::Network
{
	class SocketException
		: public OS::SystemException
	{
	public:
		using SystemException::SystemException;
	};
}
