#pragma once
#include "AL/Common.hpp"

#include "IDriver.hpp"

#include "AL/Hardware/I2C.hpp"

namespace AL::Hardware::Drivers
{
	struct AHT10Data
	{
		Float DewPoint;
		Float Humidity;
		Float Temperature;
	};

	enum class AHT10DataFlags : uint16
	{
		Humidity      = 0x1,

		DewPoint_C    = 0x10,
		DewPoint_F    = 0x20,

		Temperature_C = 0x100,
		Temperature_F = 0x200,
	};

	AL_DEFINE_ENUM_FLAG_OPERATORS(AHT10DataFlags);

	class AHT10
		: public IDriver<AHT10DataFlags, AHT10Data, Void>
	{
		Bool      isOpen = False;
		Bool      isBusAllocated = False;

		I2CBus*   lpBus;
		I2CDevice device;

	public:
		inline static const I2CAddress DEVICE_ADDRESS = 0x38;

		AHT10(AHT10&& aht10)
			: isOpen(
				aht10.isOpen
			),
			isBusAllocated(
				aht10.isBusAllocated
			),
			lpBus(
				aht10.lpBus
			),
			device(
				Move(aht10.device)
			)
		{
			aht10.isOpen = False;
			aht10.isBusAllocated = False;
		}

		AHT10(I2CBus& bus, I2CAddress address = DEVICE_ADDRESS)
			: isBusAllocated(
				False
			),
			lpBus(
				&bus
			),
			device(
				bus,
				address
			)
		{
		}

#if defined(AL_PLATFORM_PICO)
		AHT10(::i2c_inst* i2c, GPIOPin scl, GPIOPin sda, uint32 baud, I2CAddress address = DEVICE_ADDRESS)
			: isBusAllocated(
				True
			),
			lpBus(
				new I2CBus(
					i2c,
					scl,
					sda,
					baud
				)
			),
			device(
				*lpBus,
				address
			)
		{
		}
#elif defined(AL_PLATFORM_LINUX)
		AHT10(FileSystem::Path&& path, uint32 baud, I2CAddress address = DEVICE_ADDRESS)
			: isBusAllocated(
				True
			),
			lpBus(
				new I2CBus(
					Move(path),
					baud
				)
			),
			device(
				*lpBus,
				address
			)
		{
		}
		AHT10(const FileSystem::Path& path, uint32 baud, I2CAddress address = DEVICE_ADDRESS)
			: AHT10(
				FileSystem::Path(path),
				baud,
				address
			)
		{
		}
#endif

		virtual ~AHT10()
		{
			if (IsOpen())
			{

				Close();
			}

			if (isBusAllocated)
			{
				delete lpBus;

				isBusAllocated = False;
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
				"AHT10 already open"
			);

			if (isBusAllocated)
			{
				try
				{
					lpBus->Open();
				}
				catch (Exception& exception)
				{

					throw Exception(
						Move(exception),
						"Error opening I2CBus"
					);
				}
			}

			try
			{
				Device_Reset();
			}
			catch (Exception& exception)
			{
				if (isBusAllocated)
				{

					lpBus->Close();
				}

				throw Exception(
					Move(exception),
					"Error resetting device"
				);
			}

			isOpen = True;
		}

		virtual Void Close() override
		{
			if (IsOpen())
			{
				if (isBusAllocated)
				{

					lpBus->Close();
				}

				isOpen = False;
			}
		}

		// @throw AL::Exception
		virtual Void Read(Channel channel, ReadData& data) override
		{
			AL_ASSERT(
				IsOpen(),
				"AHT10 not open"
			);

			try
			{
				Device_Trigger();
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error triggering device"
				);
			}

			uint8 buffer[6];

			Float humidity;
			Float temperature;

			do
			{
				Sleep(
					TimeSpan::FromMilliseconds(10)
				);

				try
				{
					Device_Read(
						buffer
					);
				}
				catch (Exception& exception)
				{

					throw Exception(
						Move(exception),
						"Error reading device"
					);
				}
			} while (!GetHumidityAndTemperature(humidity, temperature, buffer));

			auto dewPoint = CalculateDewPoint(
				humidity,
				temperature
			);

			BitMask<AHT10DataFlags> flags(
				channel
			);

			if (flags.IsSet(AHT10DataFlags::Humidity))
			{

				data.Humidity = humidity;
			}

			if (flags.IsSet(AHT10DataFlags::DewPoint_C))
			{

				data.DewPoint = dewPoint;
			}
			else if (flags.IsSet(AHT10DataFlags::DewPoint_F))
			{

				data.DewPoint = (dewPoint * 1.8f) + 32.0f;
			}

			if (flags.IsSet(AHT10DataFlags::Temperature_C))
			{

				data.Temperature = temperature;
			}
			else if (flags.IsSet(AHT10DataFlags::Temperature_F))
			{

				data.Temperature = (temperature * 1.8f) + 32.0f;
			}
		}

		AHT10& operator = (AHT10&& aht10)
		{
			Close();

			isOpen = aht10.isOpen;
			aht10.isOpen = False;

			isBusAllocated = aht10.isBusAllocated;
			aht10.isBusAllocated = False;

			lpBus = aht10.lpBus;

			device = Move(
				aht10.device
			);

			return *this;
		}

		Bool operator == (const AHT10& aht10) const
		{
			if (device != aht10.device)
			{

				return False;
			}

			return True;
		}
		Bool operator != (const AHT10& aht10) const
		{
			if (operator==(aht10))
			{

				return False;
			}

			return True;
		}

	private:
		// @throw AL::Exception
		Void Device_Reset()
		{
			device.Write(
				0b10111010
			);

			Sleep(
				TimeSpan::FromMilliseconds(20)
			);
		}

		// @throw AL::Exception
		Void Device_Trigger()
		{
			uint8 data[] =
			{
				0b10101100,
				0b00110011,
				0b00000000
			};

			device.Write(
				data
			);
		}

		// @throw AL::Exception
		Void Device_Read(uint8(&buffer)[6]) const
		{
			device.Read(
				buffer
			);
		}

		static Float CalculateDewPoint(Float humidity, Float temperature)
		{
			auto waterVaporPressure = humidity * 0.06112f * Math::Exp((17.62f * temperature) / (temperature + 243.12f));
			return ((243.12f * Math::Log(waterVaporPressure)) - 440.1f) / (19.43f - Math::Log(waterVaporPressure));
		}

		static Bool GetHumidityAndTemperature(Float& humidity, Float& temperature, const uint8(&buffer)[6])
		{
			// bit[7] = The device is busy
			if ((buffer[0] & 0x80) == 0x80)
			{

				return False;
			}

			humidity = (static_cast<Float>((static_cast<uint32>(buffer[1]) << 12) | (static_cast<uint32>(buffer[2]) << 4) | static_cast<uint32>(buffer[3] >> 4)) * 100.0f) / 1048576.0f;
			temperature = ((static_cast<Float>((static_cast<uint32>(buffer[3] & 0x0F) << 16) | (static_cast<uint32>(buffer[4]) << 8) | static_cast<uint32>(buffer[5])) * 200.0f) / 1048576.0f) - 50.0f;

			return True;
		}
	};
}
