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

#if defined(AL_PLATFORM_PICO)
		LED_RGB(GPIOPin pin1, GPIOPin pin2, GPIOPin pin3)
			: gpioR(
				pin1,
				GPIOPinDirections::Out,
				GPIOPinValues::Low
			),
			gpioG(
				pin2,
				GPIOPinDirections::Out,
				GPIOPinValues::Low
			),
			gpioB(
				pin3,
				GPIOPinDirections::Out,
				GPIOPinValues::Low
			)
		{
		}
#elif defined(AL_PLATFORM_LINUX)
		LED_RGB(GPIOBus bus1, GPIOPin pin1, GPIOBus bus2, GPIOPin pin2, GPIOBus bus3, GPIOPin pin3)
			: gpioR(
				bus1,
				pin1,
				GPIOPinDirections::Out,
				GPIOPinValues::Low
			),
			gpioG(
				bus2,
				pin2,
				GPIOPinDirections::Out,
				GPIOPinValues::Low
			),
			gpioB(
				bus3,
				pin3,
				GPIOPinDirections::Out,
				GPIOPinValues::Low
			)
		{
		}
#endif

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

			try
			{
				gpioR.Open();
			}
			catch (AL::Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error opening GPIO (Red)"
				);
			}

			try
			{
				gpioG.Open();
			}
			catch (AL::Exception& exception)
			{
				gpioR.Close();

				throw Exception(
					Move(exception),
					"Error opening GPIO (Green)"
				);
			}

			try
			{
				gpioB.Open();
			}
			catch (AL::Exception& exception)
			{
				gpioG.Close();
				gpioR.Close();

				throw Exception(
					Move(exception),
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
			GPIOPinValues r;
			GPIOPinValues g;
			GPIOPinValues b;

			try
			{
				gpioR.Read(r);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error reading GPIO (Red)"
				);
			}

			try
			{
				gpioG.Read(g);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error reading GPIO (Green)"
				);
			}

			try
			{
				gpioB.Read(b);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
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
			try
			{
				gpioR.Write(
					(data.A && data.R) ? GPIOPinValues::High : GPIOPinValues::Low
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error writing GPIO (Red)"
				);
			}

			try
			{
				gpioG.Write(
					(data.A && data.G) ? GPIOPinValues::High : GPIOPinValues::Low
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error writing GPIO (Green)"
				);
			}

			try
			{
				gpioB.Write(
					(data.A && data.B) ? GPIOPinValues::High : GPIOPinValues::Low
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error writing GPIO (Blue)"
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
