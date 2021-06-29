#pragma once
#include "AL/DotNET/Common.hpp"

#include <AL/GPIO/UARTDeviceFlags.hpp>

namespace AL::DotNET::GPIO
{
	[::System::FlagsAttribute]
	public enum class UARTDeviceFlags
		: typename Get_Enum_Base<AL::GPIO::UARTDeviceFlags>::Type
	{
		None         = static_cast<typename Get_Enum_Base<AL::GPIO::UARTDeviceFlags>::Type>(AL::GPIO::UARTDeviceFlags::None),
		
		Parity       = static_cast<typename Get_Enum_Base<AL::GPIO::UARTDeviceFlags>::Type>(AL::GPIO::UARTDeviceFlags::Parity),
		Parity_Odd   = static_cast<typename Get_Enum_Base<AL::GPIO::UARTDeviceFlags>::Type>(AL::GPIO::UARTDeviceFlags::Parity_Odd),
		Parity_Even  = static_cast<typename Get_Enum_Base<AL::GPIO::UARTDeviceFlags>::Type>(AL::GPIO::UARTDeviceFlags::Parity_Even),

		Use2StopBits = static_cast<typename Get_Enum_Base<AL::GPIO::UARTDeviceFlags>::Type>(AL::GPIO::UARTDeviceFlags::Use2StopBits),
	};
}
