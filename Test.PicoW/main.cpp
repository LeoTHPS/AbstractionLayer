#include <AL/Common.hpp>

#include <AL/OS/Timer.hpp>
#include <AL/OS/Console.hpp>

#include <AL/OS/Pico/Core1.hpp>
#include <AL/OS/Pico/Stdio.hpp>

#include <AL/Hardware/I2C.hpp>
#include <AL/Hardware/SPI.hpp>
#include <AL/Hardware/GPIO.hpp>
#include <AL/Hardware/UART.hpp>

#include <AL/Hardware/PicoW/ADC.hpp>
#include <AL/Hardware/PicoW/CYW43.hpp>

#define WIFI_SSID      "Leo-Wireless"
#define WIFI_PASSWORD  ""
#define WIFI_AUTH_TYPE AL::Hardware::PicoW::CYW43AuthTypes::Open

// @throw AL::Exception
void do_the_thing()
{
	AL::Hardware::PicoW::CYW43::Open(
		AL::Hardware::PicoW::CYW43Countries::USA
	);

	AL::Hardware::PicoW::CYW43ScanCallback onScan(
		[](const AL::Hardware::PicoW::CYW43Network& _network)
		{
			AL::OS::Console::WriteLine(
				"[RSSI: %i, SSID: %s, BSSID: %s, Channel: %u, AuthType: %u]",
				_network.RSSI,
				_network.SSID.GetCString(),
				_network.BSSID.GetCString(),
				_network.Channel,
				_network.AuthType
			);

			return AL::True;
		}
	);

	AL::Hardware::PicoW::CYW43::Scan(
		onScan
	);

	AL::Hardware::PicoW::CYW43::Connect(
		WIFI_SSID,
		WIFI_PASSWORD,
		WIFI_AUTH_TYPE
	);

	AL::OS::Timer timer;

	do
	{
		timer.Reset();

		AL::Hardware::PicoW::CYW43::LED::Write(AL::True);
		AL::Sleep(AL::TimeSpan::FromMilliseconds(100));
		AL::Hardware::PicoW::CYW43::LED::Write(AL::False);

		while (timer.GetElapsed() < AL::TimeSpan::FromSeconds(1))
		{
			tight_loop_contents();
		}
	} while (true);

	AL::Hardware::PicoW::CYW43::Close();
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
