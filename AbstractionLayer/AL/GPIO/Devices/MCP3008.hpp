#pragma once
#include "AL/Common.hpp"

#include "SPIDevice.hpp"

namespace AL::GPIO::Devices
{
	class MCP3008
		: public SPIDevice<uint8, uint16, void>
	{
		static constexpr SPIModes    DEVICE_MODE      = SPIModes::Zero;
		static constexpr SPISpeed    DEVICE_SPEED     = 1350000;
		static constexpr SPIBitCount DEVICE_BIT_COUNT = 8;

		static constexpr SPICSModes  DEVICE_CS_MODE   = SPICSModes::ActiveLow;

	public:
		static constexpr DataR   DATA_MAX      = 0x3FF;

		static constexpr Channel CHANNEL_FIRST = 0;
		static constexpr Channel CHANNEL_LAST  = 7;
		static constexpr Channel CHANNEL_COUNT = 8;

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

		auto& operator = (MCP3008&& mcp3008)
		{
			SPIDevice::operator=(
				Move(mcp3008)
			);

			return *this;
		}

	protected:
		// @throw AL::Exceptions::Exception
		virtual void OnRead(Channel channel, DataR& value) override
		{
			uint8 rx[3];

			uint8 tx[3] =
			{
				static_cast<uint8>(0xC0 | ((channel & 0x07) << 3)),
				0,
				0
			};

			GetDevice().ReadWrite(
				rx,
				tx
			);

			value  = static_cast<DataR>((rx[0] & 0x01) << 9);
			value |= static_cast<DataR>((rx[1] & 0xFF) << 1);
			value |= static_cast<DataR>((rx[2] & 0x80) >> 7);
			value &= DATA_MAX;
		}
	};
}
