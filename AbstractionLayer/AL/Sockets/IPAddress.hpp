#pragma once
#include "AL/Common.hpp"

namespace AL::Sockets
{
	enum class AddressFamilies
	{
		IPv4,
		IPv6
	};

	template<AddressFamilies ADDRESS_FAMILY>
	class IIPAddress;
}
