#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_RP2040)
	#error Platform not supported
#endif

#include "AL/Hardware/Drivers/IDriver.hpp"

#include <hardware/adc.h>
#include <hardware/gpio.h>

namespace AL::Hardware::Pico
{
	enum class ADCChannels : uint8
	{
		ADC0 = 0x01, // GPIO 26
		ADC1 = 0x02, // GPIO 27
		ADC2 = 0x04, // GPIO 28
		ADC3 = 0x08, // GPIO 29
		ADC4 = 0x10, // Chip Temperature

		All  = ADC0 | ADC1 | ADC2 | ADC3 | ADC4
	};

	class ADC
		: public Drivers::IDriver<ADCChannels, uint16, Void>
	{
		Bool                 isOpen = False;

		BitMask<ADCChannels> channels;

	public:
		static constexpr Float CalculateTemperatureC(ReadData adcValue, Float adcVoltage = 3.3f)
		{
			return 27 - ((adcValue * (adcVoltage / 0xFFF)) - 0.706f) / 0.001721f;
		}
		static constexpr Float CalculateTemperatureF(ReadData adcValue, Float adcVoltage = 3.3f)
		{
			return (CalculateTemperatureC(adcValue, adcVoltage) * 9.0f / 5.0f) + 32.0f;
		}

		static constexpr ReadData DATA_MAX = 0xFFF;

		ADC(ADC&& adc)
			: isOpen(
				adc.isOpen
			),
			channels(
				Move(adc.channels)
			)
		{
			adc.isOpen = False;
		}

		explicit ADC(ADCChannels channels)
			: channels(
				channels
			)
		{
		}

		virtual Bool IsOpen() const override
		{
			return isOpen;
		}

		auto& GetChannels() const
		{
			return channels;
		}

		// @throw AL::Exception
		virtual Void Open() override
		{
			AL_ASSERT(
				!IsOpen(),
				"ADC already open"
			);

			::adc_init();

			if (channels.IsSet(ADCChannels::ADC0)) ::adc_gpio_init(26);
			if (channels.IsSet(ADCChannels::ADC1)) ::adc_gpio_init(27);
			if (channels.IsSet(ADCChannels::ADC2)) ::adc_gpio_init(28);
			if (channels.IsSet(ADCChannels::ADC3)) ::adc_gpio_init(29);
			if (channels.IsSet(ADCChannels::ADC4)) ::adc_set_temp_sensor_enabled(true);

			isOpen = True;
		}

		virtual Void Close() override
		{
			if (IsOpen())
			{

				isOpen = False;
			}
		}

		// @throw AL::Exception
		virtual Void Read(Channel channel, ReadData& data) override
		{
			AL_ASSERT(
				IsOpen(),
				"ADC not open"
			);

			data = 0;

			if (BitMask<ADCChannels>::IsSet(channel, ADCChannels::ADC0))
			{
				::adc_select_input(
					0
				);

				data += ::adc_read();
			}

			if (BitMask<ADCChannels>::IsSet(channel, ADCChannels::ADC1))
			{
				::adc_select_input(
					1
				);

				data += ::adc_read();
			}

			if (BitMask<ADCChannels>::IsSet(channel, ADCChannels::ADC2))
			{
				::adc_select_input(
					2
				);

				data += ::adc_read();
			}

			if (BitMask<ADCChannels>::IsSet(channel, ADCChannels::ADC3))
			{
				::adc_select_input(
					3
				);

				data += ::adc_read();
			}

			if (BitMask<ADCChannels>::IsSet(channel, ADCChannels::ADC4))
			{
				::adc_select_input(
					4
				);

				data += ::adc_read();
			}
		}

		ADC& operator = (ADC&& adc)
		{
			isOpen = adc.isOpen;
			adc.isOpen = False;

			channels = Move(
				adc.channels
			);

			return *this;
		}

		Bool operator == (const ADC& adc) const
		{
			if (IsOpen() != adc.IsOpen())
			{

				return False;
			}

			if (GetChannels() != adc.GetChannels())
			{

				return False;
			}

			return True;
		}
		Bool operator != (const ADC& adc) const
		{
			if (operator==(adc))
			{

				return False;
			}

			return True;
		}
	};

	AL_DEFINE_ENUM_FLAG_OPERATORS(ADCChannels);
}
