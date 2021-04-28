#pragma once
#include "AL/Common.hpp"

namespace AL::WiFi
{
	enum class NetworkAuthAlgorithms : uint8
	{
		None,
		Open,
		SharedKey,
		WPA,
		WPA_PSK,
		WPA_None,
		RSNA,
		RSNA_PSK,

		Undefined
	};
}
