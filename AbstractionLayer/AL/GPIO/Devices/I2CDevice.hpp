#pragma once
#include "AL/Common.hpp"

#include "Device.hpp"

#include "AL/GPIO/I2CBus.hpp"
#include "AL/GPIO/I2CDevice.hpp"

namespace AL::GPIO::Devices
{
	template<typename T_CHANNEL, typename T_DATA_READ, typename T_DATA_WRITE>
	class I2CDevice
		: public Device<T_CHANNEL, T_DATA_READ, T_DATA_WRITE>
	{
		GPIO::I2CDevice device;
		
	public:
		I2CDevice(I2CDevice&& i2cDevice)
			: Device<T_CHANNEL, T_DATA_READ, T_DATA_WRITE>(
				Move(i2cDevice)
			),
			device(
				Move(i2cDevice.device)
			)
		{
		}

		I2CDevice(GPIO::I2CBus& bus, I2CAddress address)
			: device(
				bus,
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
		}

		virtual void OnClose() override
		{
		}
	};
}
