#include <AL/Common.hpp>

#include <AL/OS/Timer.hpp>
#include <AL/OS/Console.hpp>

#include <AL/OS/Pico/Core1.hpp>
#include <AL/OS/Pico/Stdio.hpp>

#include <AL/Hardware/I2C.hpp>
#include <AL/Hardware/SPI.hpp>
#include <AL/Hardware/GPIO.hpp>
#include <AL/Hardware/UART.hpp>

#include <AL/Hardware/Pico/ADC.hpp>
#include <AL/Hardware/Pico/LED.hpp>

// @throw AL::Exception
void do_the_thing()
{
	AL::Hardware::Pico::LED::Open();

	AL::OS::Timer timer;

	do
	{
		timer.Reset();

		AL::Hardware::Pico::LED::Write(AL::True);
		AL::Sleep(AL::TimeSpan::FromMilliseconds(100));
		AL::Hardware::Pico::LED::Write(AL::False);

		while (timer.GetElapsed() < AL::TimeSpan::FromSeconds(1))
		{
			tight_loop_contents();
		}
	} while (true);

	AL::Hardware::Pico::LED::Close();
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
