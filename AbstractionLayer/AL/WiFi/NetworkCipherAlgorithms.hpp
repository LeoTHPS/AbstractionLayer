#pragma once
#include "AL/Common.hpp"

namespace AL::WiFi
{
	enum class NetworkCipherAlgorithms : uint8
	{
		None,
		WEP,
		WEP40,
		WEP104,
		TKIP,
		CCMP,

		Undefined
	};
}
