#pragma once
#include "AL/Common.hpp"

#include "IDriver.hpp"

#include "AL/Hardware/GPIO.hpp"

namespace AL::Hardware::Drivers
{
	class LED
		: public IDriver<Void, Bool, Bool>
	{
		Bool isOpen = False;

		GPIO gpio;

	public:
		LED(LED&& led)
			: isOpen(
				led.isOpen
			),
			gpio(
				Move(led.gpio)
			)
		{
			led.isOpen = False;
		}

#if defined(AL_PLATFORM_PICO)
		explicit LED(GPIOPin pin)
			: gpio(
				pin,
				GPIOPinDirections::Out,
				GPIOPinValues::Low
			)
		{
		}
#elif defined(AL_PLATFORM_LINUX)
		LED(GPIOBus bus, GPIOPin pin)
			: gpio(
				bus,
				pin,
				GPIOPinDirections::Out,
				GPIOPinValues::Low
			)
		{
		}
#endif

		virtual ~LED()
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
				"LED already open"
			);

			try
			{
				gpio.Open();
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error opening GPIO"
				);
			}

			isOpen = True;
		}

		virtual Void Close() override
		{
			if (IsOpen())
			{
				gpio.Close();

				isOpen = False;
			}
		}

		// @throw AL::Exception
		virtual Void Read(ReadData& data) override
		{
			GPIOPinValues value;

			try
			{
				gpio.Read(
					value
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error reading GPIO"
				);
			}

			data = value == GPIOPinValues::High;
		}

		// @throw AL::Exception
		virtual Void Write(const WriteData& data) override
		{
			try
			{
				gpio.Write(
					data ? GPIOPinValues::High : GPIOPinValues::Low
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error writing GPIO"
				);
			}
		}

		LED& operator = (LED&& led)
		{
			Close();

			isOpen = led.isOpen;
			led.isOpen = False;

			gpio = Move(
				led.gpio
			);

			return *this;
		}

		Bool operator == (const LED& led) const
		{
			if (gpio != led.gpio)
			{

				return False;
			}

			return True;
		}
		Bool operator != (const LED& led) const
		{
			if (operator==(led))
			{

				return False;
			}

			return True;
		}
	};
}
