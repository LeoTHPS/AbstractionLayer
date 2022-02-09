#include <AL/Common.hpp>

#include <AL/OS/Timer.hpp>
#include <AL/OS/System.hpp>

#include <AL/OS/Pico/Core1.hpp>
#include <AL/OS/Pico/Stdio.hpp>

#include <AL/Hardware/I2C.hpp>
#include <AL/Hardware/SPI.hpp>
#include <AL/Hardware/GPIO.hpp>
#include <AL/Hardware/UART.hpp>

#include <AL/Hardware/Drivers/AHT10.hpp>
#include <AL/Hardware/Drivers/AT24C256.hpp>
#include <AL/Hardware/Drivers/BMP180.hpp>
#include <AL/Hardware/Drivers/BN_180.hpp>
#include <AL/Hardware/Drivers/MCP3008.hpp>
#include <AL/Hardware/Drivers/RCWL_0516.hpp>
#include <AL/Hardware/Drivers/RFM69HCW.hpp>
#include <AL/Hardware/Drivers/Stepper_28BYJ_48.hpp>

#include <pico.h>

#include <boards/pico.h>

int main()
{
	AL::OS::Pico::Stdio::Init();

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

	return 0;
}
