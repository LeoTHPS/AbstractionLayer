#pragma once
#include "AL/Common.hpp"

#include "Device.hpp"

#include "AL/GPIO/UARTDevice.hpp"

namespace AL::GPIO::Devices
{
	template<typename T_CHANNEL, typename T_DATA>
	class UARTDevice
		: public Device<T_CHANNEL, T_DATA>
	{
		GPIO::UARTDevice device;
		String           deviceName;
		UARTDeviceSpeeds deviceSpeed;

	public:
		UARTDevice(UARTDevice&& uartDevice)
			: Device<T_CHANNEL, T_DATA>(
				Move(uartDevice)
			),
			device(
				Move(uartDevice.device)
			),
			deviceName(
				Move(uartDevice.deviceName)
			),
			deviceSpeed(
				uartDevice.deviceSpeed
			)
		{
			uartDevice.deviceSpeed = UARTDeviceSpeeds::Default;
		}

		UARTDevice(String&& deviceName, UARTDeviceSpeeds deviceSpeed)
			: deviceName(
				Move(deviceName)
			),
			deviceSpeed(
				deviceSpeed
			)
		{
		}

		auto& operator = (UARTDevice&& uartDevice)
		{
			Device<T_CHANNEL, T_DATA>::operator=(
				Move(uartDevice)
			);

			deviceName = Move(
				uartDevice.deviceName
			);

			deviceSpeed = uartDevice.deviceSpeed;
			uartDevice.deviceSpeed = UARTDeviceSpeeds::Default;

			return *this;
		}

	protected:
		auto& GetDevice()
		{
			return device;
		}
		auto& GetDevice() const
		{
			return device;
		}

		// @throw AL::Exceptions::Exception
		virtual void OnOpen() override
		{
			GPIO::UARTDevice::Open(
				device,
				deviceName,
				deviceSpeed
			);
		}

		virtual void OnClose() override
		{
			device.Close();
		}
	};
}
