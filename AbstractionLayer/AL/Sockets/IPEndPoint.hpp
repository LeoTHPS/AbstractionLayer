#pragma once
#include "AL/Common.hpp"

#include "IPAddress.hpp"

namespace AL::Sockets
{
	template<AddressFamilies ADDRESS_FAMILY>
	struct IPEndPoint
	{
		IIPAddress<ADDRESS_FAMILY> Host;
		uint16                     Port;
	};
}
