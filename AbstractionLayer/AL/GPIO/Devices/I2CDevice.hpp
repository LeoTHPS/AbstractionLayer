#pragma once
#include "AL/Common.hpp"

#include "Device.hpp"

#include "AL/GPIO/I2CDevice.hpp"

namespace AL::GPIO::Devices
{
	template<typename T_CHANNEL, typename T_DATA_READ, typename T_DATA_WRITE>
	class I2CDevice
		: public Device<T_CHANNEL, T_DATA_READ, T_DATA_WRITE>
	{
		GPIO::I2CDevice device;
		String          deviceName;
		I2CAddress      deviceAddress;

	public:
		I2CDevice(I2CDevice&& i2cDevice)
			: Device<T_CHANNEL, T_DATA_READ, T_DATA_WRITE>(
				Move(i2cDevice)
			),
			device(
				Move(i2cDevice.device)
			),
			deviceName(
				Move(i2cDevice.deviceName)
			),
			deviceAddress(
				i2cDevice.deviceAddress
			)
		{
		}

		I2CDevice(String&& name, I2CAddress address)
			: deviceName(
				Move(name)
			),
			deviceAddress(
				address
			)
		{
		}

		auto& operator = (I2CDevice&& i2cDevice)
		{
			Device<T_CHANNEL, T_DATA_READ, T_DATA_WRITE>::operator=(
				Move(i2cDevice)
			);

			device = Move(
				i2cDevice.device
			);

			deviceName = Move(
				i2cDevice.deviceName
			);

			deviceAddress = i2cDevice.deviceAddress;

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
				GPIO::I2CDevice::Open(
					device,
					deviceName,
					deviceAddress
				);
			}
			catch (Exceptions::Exception& exception)
			{

				throw Exceptions::Exception(
					Move(exception),
					"Error opening GPIO::I2CDevice"
				);
			}
		}

		virtual void OnClose() override
		{
			device.Close();
		}
	};
}
