#pragma once
#include "AL/Common.hpp"

#include "IPAddress.hpp"

namespace AL::Network
{
	struct IPEndPoint
	{
		IPAddress Host;
		uint16    Port;
	};
}
