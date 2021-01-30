#pragma once
#include "AL/Common.hpp"

#include "SPIDevice.hpp"

namespace AL::GPIO::Devices
{
	class MCP3008
		: public SPIDevice<uint8, uint16>
	{
		static constexpr SPIModes    DEVICE_MODE      = SPIModes::Zero;
		static constexpr SPISpeed    DEVICE_SPEED     = 1350000;
		static constexpr SPIBitCount DEVICE_BIT_COUNT = 8;

		static constexpr SPICSModes  DEVICE_CS_MODE   = SPICSModes::ActiveLow;

	public:
		MCP3008(MCP3008&& mcp3008)
			: SPIDevice(
				Move(mcp3008)
			)
		{
		}

		MCP3008(SPIBusId busId, SPIDeviceId deviceId)
			: SPIDevice(
				busId,
				deviceId,
				DEVICE_MODE,
				DEVICE_SPEED,
				DEVICE_BIT_COUNT
			)
		{
		}

		MCP3008(SPIBusId busId, DeviceId csPinDeviceId, PinNumber csPinNumber)
			: SPIDevice(
				busId,
				csPinDeviceId,
				csPinNumber,
				DEVICE_CS_MODE,
				DEVICE_MODE,
				DEVICE_SPEED,
				DEVICE_BIT_COUNT
			)
		{
		}

	protected:
		// @throw AL::Exceptions::Exception
		virtual void OnRead(Channel channel, Data& value) override
		{
			uint8 rx[3];

			uint8 tx[3] =
			{
				static_cast<uint8>(0x18 | (static_cast<uint8>(channel) & 0x3)),
				0,
				0
			};

			GetDevice().ReadWrite(
				rx,
				tx
			);

			value = ((static_cast<uint16>(rx[1]) & 0x0003) << 8) | (static_cast<uint16>(rx[2]) & 0x00FF);
		}
	};
}
