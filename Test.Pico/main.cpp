#include <AL/Common.hpp>

#include <AL/OS/Timer.hpp>
#include <AL/OS/Console.hpp>

#include <AL/OS/Pico/Core1.hpp>
#include <AL/OS/Pico/Stdio.hpp>

#include <AL/Hardware/I2C.hpp>
#include <AL/Hardware/SPI.hpp>
#include <AL/Hardware/GPIO.hpp>
#include <AL/Hardware/UART.hpp>

#include <pico.h>

#include <boards/pico.h>

// @throw AL::Exception
void do_the_thing()
{
	AL::Hardware::GPIO pin(
		PICO_DEFAULT_LED_PIN
	);

	pin.Open();

	AL::OS::Timer timer;

	do
	{
		timer.Reset();

		pin.Write(AL::Hardware::GPIOPinValues::High);
		AL::Sleep(AL::TimeSpan::FromMilliseconds(100));
		pin.Write(AL::Hardware::GPIOPinValues::Low);

		while (timer.GetElapsed() < AL::TimeSpan::FromSeconds(1))
		{
			tight_loop_contents();
		}
	} while (true);

	pin.Close();
}

int main()
{
	AL::OS::Pico::Stdio::Init();

	try
	{
		do_the_thing();
	}
	catch (const AL::Exception& exception)
	{
		AL::OS::Console::WriteLine(
			exception.GetMessage()
		);

		if (auto lpInnerException = exception.GetInnerException())
		{
			do
			{
				AL::OS::Console::WriteLine(
					lpInnerException->GetMessage()
				);
			} while ((lpInnerException = lpInnerException->GetInnerException()) != nullptr);
		}
	}

	return 0;
}
