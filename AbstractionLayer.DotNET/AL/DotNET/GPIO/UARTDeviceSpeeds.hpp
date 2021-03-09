#pragma once
#include "AL/DotNET/Common.hpp"

#include <AL/GPIO/UARTDeviceSpeeds.hpp>

namespace AL::DotNET::GPIO
{
	public enum class UARTDeviceSpeeds
		: typename Get_Enum_Base<AL::GPIO::UARTDeviceSpeeds>::Type
	{
		Default     = static_cast<typename Get_Enum_Base<AL::GPIO::UARTDeviceSpeeds>::Type>(AL::GPIO::UARTDeviceSpeeds::Default),

		Baud_110    = static_cast<typename Get_Enum_Base<AL::GPIO::UARTDeviceSpeeds>::Type>(AL::GPIO::UARTDeviceSpeeds::Baud_110),
		Baud_300    = static_cast<typename Get_Enum_Base<AL::GPIO::UARTDeviceSpeeds>::Type>(AL::GPIO::UARTDeviceSpeeds::Baud_300),
		Baud_600    = static_cast<typename Get_Enum_Base<AL::GPIO::UARTDeviceSpeeds>::Type>(AL::GPIO::UARTDeviceSpeeds::Baud_600),
		Baud_1200   = static_cast<typename Get_Enum_Base<AL::GPIO::UARTDeviceSpeeds>::Type>(AL::GPIO::UARTDeviceSpeeds::Baud_1200),
		Baud_2400   = static_cast<typename Get_Enum_Base<AL::GPIO::UARTDeviceSpeeds>::Type>(AL::GPIO::UARTDeviceSpeeds::Baud_2400),
		Baud_4800   = static_cast<typename Get_Enum_Base<AL::GPIO::UARTDeviceSpeeds>::Type>(AL::GPIO::UARTDeviceSpeeds::Baud_4800),
		Baud_9600   = static_cast<typename Get_Enum_Base<AL::GPIO::UARTDeviceSpeeds>::Type>(AL::GPIO::UARTDeviceSpeeds::Baud_9600),
		Baud_14400  = static_cast<typename Get_Enum_Base<AL::GPIO::UARTDeviceSpeeds>::Type>(AL::GPIO::UARTDeviceSpeeds::Baud_14400),
		Baud_19200  = static_cast<typename Get_Enum_Base<AL::GPIO::UARTDeviceSpeeds>::Type>(AL::GPIO::UARTDeviceSpeeds::Baud_19200),
		Baud_38400  = static_cast<typename Get_Enum_Base<AL::GPIO::UARTDeviceSpeeds>::Type>(AL::GPIO::UARTDeviceSpeeds::Baud_38400),
		Baud_56000  = static_cast<typename Get_Enum_Base<AL::GPIO::UARTDeviceSpeeds>::Type>(AL::GPIO::UARTDeviceSpeeds::Baud_56000),
		Baud_57600  = static_cast<typename Get_Enum_Base<AL::GPIO::UARTDeviceSpeeds>::Type>(AL::GPIO::UARTDeviceSpeeds::Baud_57600),
		Baud_115200 = static_cast<typename Get_Enum_Base<AL::GPIO::UARTDeviceSpeeds>::Type>(AL::GPIO::UARTDeviceSpeeds::Baud_115200),
		Baud_128000 = static_cast<typename Get_Enum_Base<AL::GPIO::UARTDeviceSpeeds>::Type>(AL::GPIO::UARTDeviceSpeeds::Baud_128000),
		Baud_256000 = static_cast<typename Get_Enum_Base<AL::GPIO::UARTDeviceSpeeds>::Type>(AL::GPIO::UARTDeviceSpeeds::Baud_256000)
	};
}
