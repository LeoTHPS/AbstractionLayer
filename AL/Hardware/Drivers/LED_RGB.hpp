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
		Bool isOpen = False;

		GPIO gpioR;
		GPIO gpioG;
		GPIO gpioB;

	public:
		LED_RGB(LED_RGB&& led_rgb)
			: isOpen(
				led_rgb.isOpen
			),
			gpioR(
				Move(led_rgb.gpioR)
			),
			gpioG(
				Move(led_rgb.gpioG)
			),
			gpioB(
				Move(led_rgb.gpioB)
			)
		{
			led_rgb.isOpen = False;
		}

		LED_RGB(GPIOBus bus1, GPIOPin pin1, GPIOBus bus2, GPIOPin pin2, GPIOBus bus3, GPIOPin pin3)
			: gpioR(
				bus1,
				pin1
			),
			gpioG(
				bus2,
				pin2
			),
			gpioB(
				bus3,
				pin3
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

			auto GPIO_Open = [](GPIO& _gpio)
			{
				try
				{
					_gpio.Open();
				}
				catch (Exception& exception)
				{

					throw Exception(
						Move(exception),
						"Error opening GPIO [Bus: %u, Pin: %u]",
						_gpio.GetBus(),
						_gpio.GetPin()
					);
				}

				try
				{
					_gpio.SetDirection(
						GPIOPinDirections::Out,
						GPIOPinValues::Low
					);
				}
				catch (Exception& exception)
				{
					_gpio.Close();

					throw Exception(
						Move(exception),
						"Error setting GPIO direction [Bus: %u, Pin: %u]",
						_gpio.GetBus(),
						_gpio.GetPin()
					);
				}
			};

			try
			{
				GPIO_Open(
					gpioR
				);
			}
			catch (AL::Exception& exception)
			{

				throw AL::Exception(
					AL::Move(exception),
					"Error opening GPIO (Red)"
				);
			}

			try
			{
				GPIO_Open(
					gpioG
				);
			}
			catch (AL::Exception& exception)
			{
				gpioR.Close();

				throw AL::Exception(
					AL::Move(exception),
					"Error opening GPIO (Green)"
				);
			}

			try
			{
				GPIO_Open(
					gpioB
				);
			}
			catch (AL::Exception& exception)
			{
				gpioG.Close();
				gpioR.Close();

				throw AL::Exception(
					AL::Move(exception),
					"Error opening GPIO (Blue)"
				);
			}

			isOpen = True;
		}

		virtual Void Close() override
		{
			if (IsOpen())
			{
				gpioB.Close();
				gpioG.Close();
				gpioR.Close();

				isOpen = False;
			}
		}

		// @throw AL::Exception
		virtual Void Read(ReadData& data) override
		{
			auto GPIO_Read = [](GPIO& _gpio, GPIOPinValues& _value)
			{
				try
				{
					_gpio.Read(
						_value
					);
				}
				catch (Exception& exception)
				{

					throw Exception(
						Move(exception),
						"Error reading GPIO [Bus: %u, Pin: %u]",
						_gpio.GetBus(),
						_gpio.GetPin()
					);
				}
			};

			GPIOPinValues r;
			GPIOPinValues g;
			GPIOPinValues b;

			try
			{
				GPIO_Read(
					gpioR,
					r
				);
			}
			catch (AL::Exception& exception)
			{

				throw AL::Exception(
					AL::Move(exception),
					"Error reading GPIO (Red)"
				);
			}

			try
			{
				GPIO_Read(
					gpioG,
					g
				);
			}
			catch (AL::Exception& exception)
			{

				throw AL::Exception(
					AL::Move(exception),
					"Error reading GPIO (Green)"
				);
			}

			try
			{
				GPIO_Read(
					gpioB,
					b
				);
			}
			catch (AL::Exception& exception)
			{

				throw AL::Exception(
					AL::Move(exception),
					"Error reading GPIO (Blue)"
				);
			}

			data.A = 0xFF;
			data.R = (r == GPIOPinValues::Low) ? 0x00 : 0xFF;
			data.G = (g == GPIOPinValues::Low) ? 0x00 : 0xFF;
			data.B = (b == GPIOPinValues::Low) ? 0x00 : 0xFF;
		}

		// @throw AL::Exception
		virtual Void Write(const WriteData& data) override
		{
			auto GPIO_Write = [](GPIO& _gpio, GPIOPinValues _value)
			{
				try
				{
					_gpio.Write(
						_value
					);
				}
				catch (Exception& exception)
				{

					throw Exception(
						Move(exception),
						"Error writing GPIO [Bus: %u, Pin: %u]",
						_gpio.GetBus(),
						_gpio.GetPin()
					);
				}
			};

			try
			{
				GPIO_Write(
					gpioR,
					(data.A && data.R) ? GPIOPinValues::High : GPIOPinValues::Low
				);
			}
			catch (AL::Exception& exception)
			{

				throw AL::Exception(
					AL::Move(exception),
					"Error reading GPIO (Red)"
				);
			}

			try
			{
				GPIO_Write(
					gpioG,
					(data.A && data.G) ? GPIOPinValues::High : GPIOPinValues::Low
				);
			}
			catch (AL::Exception& exception)
			{

				throw AL::Exception(
					AL::Move(exception),
					"Error reading GPIO (Green)"
				);
			}

			try
			{
				GPIO_Write(
					gpioB,
					(data.A && data.B) ? GPIOPinValues::High : GPIOPinValues::Low
				);
			}
			catch (AL::Exception& exception)
			{

				throw AL::Exception(
					AL::Move(exception),
					"Error reading GPIO (Blue)"
				);
			}
		}

		LED_RGB& operator = (LED_RGB&& led_rgb)
		{
			Close();

			isOpen = led_rgb.isOpen;
			led_rgb.isOpen = False;

			gpioR = Move(
				led_rgb.gpioR
			);

			gpioG = Move(
				led_rgb.gpioG
			);

			gpioB = Move(
				led_rgb.gpioB
			);

			return *this;
		}

		Bool operator == (const LED_RGB& led_rgb) const
		{
			if (gpioR != led_rgb.gpioR)
			{

				return False;
			}

			if (gpioG != led_rgb.gpioG)
			{

				return False;
			}

			if (gpioB != led_rgb.gpioB)
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
