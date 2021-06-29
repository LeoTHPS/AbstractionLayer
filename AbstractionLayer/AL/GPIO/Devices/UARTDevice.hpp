#pragma once
#include "AL/Common.hpp"

#include "Device.hpp"

#include "AL/GPIO/UARTDevice.hpp"

namespace AL::GPIO::Devices
{
	template<typename T_CHANNEL, typename T_DATA_READ, typename T_DATA_WRITE>
	class UARTDevice
		: public Device<T_CHANNEL, T_DATA_READ, T_DATA_WRITE>
	{
		GPIO::UARTDevice device;
		String           deviceName;
		UARTDeviceFlags  deviceFlags;
		UARTDeviceSpeeds deviceSpeed;

	public:
		UARTDevice(UARTDevice&& uartDevice)
			: Device<T_CHANNEL, T_DATA_READ, T_DATA_WRITE>(
				Move(uartDevice)
			),
			device(
				Move(uartDevice.device)
			),
			deviceName(
				Move(uartDevice.deviceName)
			),
			deviceFlags(
				uartDevice.deviceFlags
			),
			deviceSpeed(
				uartDevice.deviceSpeed
			)
		{
			uartDevice.deviceFlags = UARTDeviceFlags::None;
			uartDevice.deviceSpeed = UARTDeviceSpeeds::Default;
		}

		UARTDevice(String&& deviceName, UARTDeviceSpeeds deviceSpeed, UARTDeviceFlags flags = UARTDeviceFlags::None)
			: deviceName(
				Move(deviceName)
			),
			deviceFlags(
				flags
			),
			deviceSpeed(
				deviceSpeed
			)
		{
		}

		auto& operator = (UARTDevice&& uartDevice)
		{
			Device<T_CHANNEL, T_DATA_READ, T_DATA_WRITE>::operator=(
				Move(uartDevice)
			);

			deviceName = Move(
				uartDevice.deviceName
			);

			deviceSpeed = uartDevice.deviceSpeed;
			uartDevice.deviceSpeed = UARTDeviceSpeeds::Default;

			deviceFlags = uartDevice.deviceFlags;
			uartDevice.deviceFlags = UARTDeviceFlags::None;

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
			try
			{
				GPIO::UARTDevice::Open(
					device,
					deviceName,
					deviceSpeed,
					deviceFlags
				);
			}
			catch (Exceptions::Exception& exception)
			{

				throw Exceptions::Exception(
					Move(exception),
					"Error opening GPIO::UARTDevice"
				);
			}
		}

		virtual void OnClose() override
		{
			device.Close();
		}
	};
}
