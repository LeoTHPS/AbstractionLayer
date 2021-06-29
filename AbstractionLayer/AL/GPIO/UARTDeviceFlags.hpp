#pragma once
#include "AL/Common.hpp"

namespace AL::GPIO
{
	enum class UARTDeviceFlags : uint8
	{
		None         = 0x0,

		Parity       = 0x1,
		Parity_Odd   = Parity | 0x2,
		Parity_Even  = Parity | 0x4,

		Use2StopBits = 0x10
	};

	AL_DEFINE_ENUM_FLAG_OPERATORS(UARTDeviceFlags);
}
