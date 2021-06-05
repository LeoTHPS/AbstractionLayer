#pragma once
#include "AL/Common.hpp"

#include "I2CDevice.hpp"

namespace AL::GPIO::Devices
{
	struct AHT10Data
	{
		float DewPoint;
		float Humidity;
		float Temperature;
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
		: public I2CDevice<AHT10DataFlags, AHT10Data, void>
	{
	public:
		AHT10(AHT10&& aht10)
			: I2CDevice(
				Move(aht10)
			)
		{
		}

		AHT10(GPIO::I2CBus& bus, I2CAddress address = 0x38)
			: I2CDevice(
				bus,
				address
			)
		{
		}

		auto& operator = (AHT10&& aht10)
		{
			I2CDevice::operator=(
				Move(aht10)
			);

			return *this;
		}

	protected:
		// @throw AL::Exceptions::Exception
		virtual void OnOpen() override
		{
			I2CDevice::OnOpen();

			// Reset device
			{
				try
				{
					GetDevice().Write(
						0b10111010
					);
				}
				catch (Exceptions::Exception& exception)
				{

					throw Exceptions::Exception(
						Move(exception),
						"Error resetting device"
					);
				}

				Sleep(
					TimeSpan::FromMilliseconds(20)
				);
			}
		}

		// @throw AL::Exceptions::Exception
		virtual void OnRead(Channel channel, DataR& value) override
		{
			BitMask<AHT10DataFlags> flags(
				channel
			);

			// Trigger device
			{
				uint8 trigger[] =
				{
					0b10101100,
					0b00110011,
					0b00000000
				};

				try
				{
					GetDevice().Write(
						trigger
					);
				}
				catch (Exceptions::Exception& exception)
				{

					throw Exceptions::Exception(
						Move(exception),
						"Error triggering device"
					);
				}

				Sleep(
					TimeSpan::FromMilliseconds(75)
				);
			}

			float fHumidity;
			float fDewPoint;
			float fTemperature;

			// Read device
			{
				uint8 buffer[6];

				try
				{
					GetDevice().Read(
						buffer
					);
				}
				catch (Exceptions::Exception& exception)
				{

					throw Exceptions::Exception(
						Move(exception),
						"Error reading device"
					);
				}

				fHumidity = (static_cast<float>((static_cast<uint32>(buffer[1]) << 12) | (static_cast<uint32>(buffer[2]) << 4) | static_cast<uint32>(buffer[3] >> 4)) * 100.0f) / 1048576.0f;
				fTemperature = ((static_cast<float>((static_cast<uint32>(buffer[3] & 0x0F) << 16) | (static_cast<uint32>(buffer[4]) << 8) | static_cast<uint32>(buffer[5])) * 200.0f) / 1048576.0f) - 50.0f;

				float fVaporPressure = fHumidity * 0.06112f * Math::Exp((17.62f * fTemperature) / (fTemperature + 243.12f));
				fDewPoint = (243.12f * Math::Log(fVaporPressure) - 440.1f) / (19.43f - Math::Log(fVaporPressure));
			}

			if (flags.IsSet(AHT10DataFlags::Humidity))
			{
				value.Humidity = fHumidity;
			}

			if (flags.IsSet(AHT10DataFlags::DewPoint_C))
			{
				value.DewPoint = fDewPoint;
			}
			else if (flags.IsSet(AHT10DataFlags::DewPoint_F))
			{
				value.DewPoint = (fDewPoint * 1.8f) + 32.0f;
			}
			
			if (flags.IsSet(AHT10DataFlags::Temperature_C))
			{
				value.Temperature = fTemperature;
			}
			else if (flags.IsSet(AHT10DataFlags::Temperature_F))
			{
				value.Temperature = (fTemperature * 1.8f) + 32.0f;
			}
		}
	};
}
