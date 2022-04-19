#pragma once
#include "AL/Common.hpp"

#include "IDriver.hpp"

#include "AL/Hardware/GPIO.hpp"

#include "AL/Drawing/Color.hpp"
#include "AL/Drawing/Colors.hpp"

#include "AL/Collections/Array.hpp"

namespace AL::Hardware::Drivers
{
	class LED_RGB
		: public IDriver<Void, Drawing::Color, Drawing::Color>
	{
		Bool                        isOpen = False;

		Collections::Array<GPIO[3]> gpio;

	public:
		LED_RGB(LED_RGB&& led_rgb)
			: isOpen(
				led_rgb.isOpen
			),
			gpio(
				Move(led_rgb.gpio)
			)
		{
			led_rgb.isOpen = False;
		}

		LED_RGB(GPIOBus bus1, GPIOPin pin1, GPIOBus bus2, GPIOPin pin2, GPIOBus bus3, GPIOPin pin3)
			: gpio(
				{
					GPIO(bus1, pin1),
					GPIO(bus2, pin2),
					GPIO(bus3, pin3)
				}
			)
		{
		}

		virtual ~LED_RGB()
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
				"LED_RGB already open"
			);

			for (auto it = gpio.begin(); it != gpio.end(); ++it)
			{
				auto lpGPIO = &(*it);

				try
				{
					lpGPIO->Open();
				}
				catch (Exception& exception)
				{
					while (it-- != gpio.begin())
					{

						it->Close();
					}

					throw Exception(
						Move(exception),
						"Error opening GPIO [Bus: %u, Pin: %u]",
						lpGPIO->GetBus(),
						lpGPIO->GetPin()
					);
				}

				try
				{
					lpGPIO->SetDirection(
						GPIOPinDirections::Out,
						GPIOPinValues::Low
					);
				}
				catch (Exception& exception)
				{
					lpGPIO->Close();

					while (it-- != gpio.begin())
					{

						it->Close();
					}

					throw Exception(
						Move(exception),
						"Error setting GPIO direction [Bus: %u, Pin: %u]",
						lpGPIO->GetBus(),
						lpGPIO->GetPin()
					);
				}
			}

			isOpen = True;
		}

		virtual Void Close() override
		{
			if (IsOpen())
			{
				for (auto it = gpio.rbegin(); it != gpio.rend(); ++it)
				{
					it->Close();
				}

				isOpen = False;
			}
		}

		// @throw AL::Exception
		virtual Void Read(ReadData& data) override
		{
			Collections::Array<GPIOPinValue[3]> values;

			for (size_t i = 0; i < gpio.GetCapacity(); ++i)
			{
				try
				{
					gpio[i].Read(
						values[i]
					);
				}
				catch (Exception& exception)
				{

					throw Exception(
						Move(exception),
						"Error reading GPIO [Bus: %u, Pin: %u]",
						gpio[i].GetBus(),
						gpio[i].GetPin()
					);
				}
			}

			data.A = 0xFF;
			data.R = values[0] ? 0xFF : 0x00;
			data.G = values[1] ? 0xFF : 0x00;
			data.B = values[2] ? 0xFF : 0x00;
		}

		// @throw AL::Exception
		virtual Void Write(const WriteData& data) override
		{
			Collections::Array<GPIOPinValue[3]> values(
				{
					GPIOPinValue((data.A && data.R) ? 1 : 0),
					GPIOPinValue((data.A && data.G) ? 1 : 0),
					GPIOPinValue((data.A && data.B) ? 1 : 0)
				}
			);

			for (size_t i = 0; i < gpio.GetCapacity(); ++i)
			{
				try
				{
					gpio[i].Write(
						values[i]
					);
				}
				catch (Exception& exception)
				{

					throw Exception(
						Move(exception),
						"Error writing GPIO [Bus: %u, Pin: %u]",
						gpio[i].GetBus(),
						gpio[i].GetPin()
					);
				}
			}
		}

		LED_RGB& operator = (LED_RGB&& led_rgb)
		{
			Close();

			isOpen = led_rgb.isOpen;
			led_rgb.isOpen = False;

			gpio = Move(
				led_rgb.gpio
			);

			return *this;
		}

		Bool operator == (const LED_RGB& led_rgb) const
		{
			if (gpio != led_rgb.gpio)
			{

				return False;
			}

			return True;
		}
		Bool operator != (const LED_RGB& led_rgb) const
		{
			if (operator==(led_rgb))
			{

				return False;
			}

			return True;
		}
	};
}
