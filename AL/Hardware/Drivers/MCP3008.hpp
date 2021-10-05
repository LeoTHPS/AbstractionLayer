#pragma once
#include "AL/Common.hpp"

#include "IDevice.hpp"

#include "AL/Hardware/SPI.hpp"

namespace AL::Hardware::Drivers
{
	class MCP3008
		: public IDevice<uint8, uint16, Void>
	{
		Bool      isOpen = False;

		SPIDevice device;

	public:
		static constexpr SPISpeed DEFAULT_SPEED = 1350000;

		static constexpr ReadData DATA_MAX      = 0x3FF;

		static constexpr Channel  CHANNEL_FIRST = 0;
		static constexpr Channel  CHANNEL_LAST  = 7;
		static constexpr Channel  CHANNEL_COUNT = 8;

		MCP3008(MCP3008&& mcp3008)
			: isOpen(
				mcp3008.isOpen
			),
			device(
				Move(mcp3008.device)
			)
		{
			mcp3008.isOpen = False;
		}

		MCP3008(String&& path, SPISpeed speed = DEFAULT_SPEED)
			: device(
				Move(path),
				SPIModes::Zero,
				speed,
				8
			)
		{
		}
		MCP3008(const String& path, SPISpeed speed = DEFAULT_SPEED)
			: MCP3008(
				String(path),
				speed
			)
		{
		}

		virtual ~MCP3008()
		{
			if (IsOpen())
			{

				Close();
			}
		}

		virtual Bool IsOpen() const override
		{
			return isOpen;
		}

		// @throw AL::Exception
		virtual Void Open() override
		{
			AL_ASSERT(
				!IsOpen(),
				"MCP3008 already open"
			);

			try
			{
				device.Open();
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error opening device"
				);
			}

			isOpen = True;
		}

		virtual Void Close() override
		{
			if (IsOpen())
			{
				device.Close();

				isOpen = False;
			}
		}

		// @throw AL::Exception
		virtual Void Read(Channel channel, ReadData& data) override
		{
			AL_ASSERT(
				IsOpen(),
				"MCP3008 not open"
			);

			uint8 rx[3];

			uint8 tx[3] =
			{
				static_cast<uint8>(0xC0 | ((channel & 0x07) << 3)),
				0,
				0
			};

			device.ReadWrite(
				rx,
				tx
			);

			data  = static_cast<ReadData>((rx[0] & 0x01) << 9);
			data |= static_cast<ReadData>((rx[1] & 0xFF) << 1);
			data |= static_cast<ReadData>((rx[2] & 0x80) >> 7);
			data &= DATA_MAX;
		}

		MCP3008& operator = (MCP3008&& mcp3008)
		{
			Close();

			isOpen = mcp3008.isOpen;
			mcp3008.isOpen = False;

			device = Move(
				mcp3008.device
			);

			return *this;
		}

		Bool operator == (const MCP3008& mcp3008) const
		{
			if (device != mcp3008.device)
			{

				return False;
			}

			return True;
		}
		Bool operator != (const MCP3008& mcp3008) const
		{
			if (operator==(mcp3008))
			{

				return False;
			}

			return True;
		}
	};
}
