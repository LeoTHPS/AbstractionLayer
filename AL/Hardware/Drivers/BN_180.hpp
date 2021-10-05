#pragma once
#include "AL/Common.hpp"

#include "IDevice.hpp"

#include "AL/Hardware/UART.hpp"

namespace AL::Hardware::Drivers
{
	class BN_180
		: public IDevice<Void, String, Void>
	{
		Bool       isOpen = False;

		UARTDevice device;

	public:
		BN_180(BN_180&& bn180);

		explicit BN_180(String&& path)
			: device(
				Move(path),
				UARTDeviceSpeeds::Baud_9600,
				UARTDeviceFlags::None
			)
		{
		}
		explicit BN_180(const String& path)
			: BN_180(
				String(path)
			)
		{
		}

		virtual ~BN_180()
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
				"BN_180 already open"
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
		virtual Void Read(ReadData& data) override
		{
			AL_ASSERT(
				IsOpen(),
				"BN_180 not open"
			);

			ReadData::Char c;

			if (data.GetSize())
			{
				data.SetCapacity(
					0
				);
			}

			bool nmeaStarted = false;

			for (size_t i = 0; device.IsOpen(); )
			{
				device.Read(
					c
				);

				if (!nmeaStarted && (c == '$'))
				{

					nmeaStarted = true;
				}

				if (nmeaStarted)
				{
					data.Append(
						c
					);

					if (i && (data[i - 1] == '\r') && (data[i] == '\n'))
					{

						break;
					}

					++i;
				}
			}
		}

		BN_180& operator = (BN_180&& bn180)
		{
			Close();

			isOpen = bn180.isOpen;
			bn180.isOpen = False;

			device = Move(
				bn180.device
			);

			return *this;
		}

		Bool operator == (const BN_180& bn180) const
		{
			if (device != bn180.device)
			{

				return False;
			}

			return True;
		}
		Bool operator != (const BN_180& bn180) const
		{
			if (operator==(bn180))
			{

				return False;
			}

			return True;
		}
	};
}
