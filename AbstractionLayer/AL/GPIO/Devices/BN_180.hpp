#pragma once
#include "AL/Common.hpp"

#include "UARTDevice.hpp"

namespace AL::GPIO::Devices
{
	class BN_180
		: public UARTDevice<void, String>
	{
	public:
		BN_180(BN_180&& bn180)
			: UARTDevice(
				Move(bn180)
			)
		{
		}

		explicit BN_180(String&& deviceName)
			: UARTDevice(
				Move(deviceName),
				UARTDeviceSpeeds::Baud_9600
			)
		{
		}

		auto& operator = (BN_180&& bn180)
		{
			UARTDevice::operator=(
				Move(bn180)
			);

			return *this;
		}

	protected:
		// @throw AL::Exceptions::Exception
		virtual void OnRead(Data& value) override
		{
			Data::Char c;

			if (value.GetSize())
			{
				value.SetSize(
					0
				);
			}

			for (size_t i = 0; GetDevice().IsOpen(); ++i)
			{
				if (!GetDevice().Read(&c, sizeof(Data::Char)))
				{

					continue;
				}

				value.Append(
					c
				);

				if (i && (value[i - 1] == '\r') && (value[i] == '\n'))
				{

					break;
				}
			}
		}
	};
}
