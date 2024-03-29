#pragma once
#include "AL/Common.hpp"

#include "IDriver.hpp"

#include "AL/Hardware/UART.hpp"

namespace AL::Hardware::Drivers
{
	class BN_180
		: public IDriver<Void, String, Void>
	{
		Bool       isOpen = False;

		UARTDevice device;

	public:
		static constexpr uint32 DEFAULT_SPEED = 9600;

		BN_180(BN_180&& bn180)
			: isOpen(
				bn180.isOpen
			),
			device(
				Move(bn180.device)
			)
		{
			bn180.isOpen = False;
		}

#if defined(AL_PLATFORM_PICO)
		BN_180(::uart_inst* uart, GPIOPin rx, GPIOPin tx, uint32 speed = DEFAULT_SPEED)
			: device(
				uart,
				rx,
				tx,
				speed,
				UARTDeviceFlags::None
			)
		{
		}
#elif defined(AL_PLATFORM_LINUX)
		BN_180(FileSystem::Path&& path, uint32 speed = DEFAULT_SPEED)
			: device(
				Move(path),
				speed,
				UARTDeviceFlags::None
			)
		{
		}
		BN_180(const FileSystem::Path& path, uint32 speed = DEFAULT_SPEED)
			: BN_180(
				FileSystem::Path(path),
				speed
			)
		{
		}
#elif defined(AL_PLATFORM_WINDOWS)
		BN_180(FileSystem::Path&& path, uint32 speed = DEFAULT_SPEED)
			: device(
				Move(path),
				speed,
				UARTDeviceFlags::None
			)
		{
		}
		BN_180(const FileSystem::Path& path, uint32 speed = DEFAULT_SPEED)
			: BN_180(
				FileSystem::Path(path),
				speed
			)
		{
		}
#endif

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

			if (data.GetSize() != 0)
			{
				data.SetCapacity(
					0
				);
			}

			Bool nmeaStarted = False;

			for (size_t i = 0; device.IsOpen(); )
			{
				device.Read(
					c
				);

				if (!nmeaStarted && (c == '$'))
				{

					nmeaStarted = True;
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

		// @throw AL::Exception
		virtual Bool TryRead(ReadData& data)
		{
			AL_ASSERT(
				IsOpen(),
				"BN_180 not open"
			);

			ReadData::Char c;

			if (data.GetSize() != 0)
			{
				data.SetCapacity(
					0
				);
			}

			Bool nmeaStarted = False;

			for (size_t i = 0; device.IsOpen(); )
			{
				while (!device.TryRead(c))
				{
					if (i == 0)
					{

						return False;
					}
				}

				if (!nmeaStarted && (c == '$'))
				{

					nmeaStarted = True;
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

			return True;
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
