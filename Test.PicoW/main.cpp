#include <AL/Common.hpp>

#include <AL/OS/Timer.hpp>
#include <AL/OS/Console.hpp>

#include <AL/OS/Pico/Stdio.hpp>

#include <AL/Network/HTTP/Request.hpp>

#include <AL/Hardware/Drivers/PicoW/CYW43.hpp>

#define WIFI_DEFAULT_SSID       "ssid"
#define WIFI_DEFAULT_PASSWORD   "passwd"
#define WIFI_DEFAULT_AUTH_TYPE  AL::Hardware::Drivers::PicoW::CYW43AuthTypes::WPA2_AES

// @throw AL::Exception
void wifi_scan(AL::Hardware::Drivers::PicoW::CYW43Network& network, AL::Bool& networkFound)
{
	AL::OS::Console::WriteLine(
		"Scanning for open WiFi networks"
	);

	networkFound = AL::False;

	AL::Hardware::Drivers::PicoW::CYW43ScanCallback onScan(
		[&network, &networkFound](const AL::Hardware::Drivers::PicoW::CYW43Network& _network)
		{
			if (_network.AuthType == AL::Hardware::Drivers::PicoW::CYW43AuthTypes::Open)
			{
				AL::OS::Console::WriteLine(
					"Found open network %s [BSSID: %s, Channel: %u, RSSI: %i]",
					_network.SSID.GetCString(),
					_network.BSSID.GetCString(),
					_network.Channel,
					_network.RSSI
				);

				network = _network;
				networkFound = AL::True;

				return AL::False;
			}

			AL::OS::Console::WriteLine(
				"Found closed network %s [BSSID: %s, Channel: %u, RSSI: %i, AuthType: %u]",
				_network.SSID.GetCString(),
				_network.BSSID.GetCString(),
				_network.Channel,
				_network.RSSI,
				_network.AuthType
			);

			return AL::True;
		}
	);

	AL::Hardware::Drivers::PicoW::CYW43::Scan(
		onScan
	);
}

// @throw AL::Exception
void wifi_connect(const AL::Hardware::Drivers::PicoW::CYW43Network& network, AL::Bool networkFound)
{
	AL::OS::Console::WriteLine(
		"Connecting to %s",
		networkFound ? network.SSID.GetCString() : WIFI_DEFAULT_SSID
	);

	if (networkFound)
	{
		AL::Hardware::Drivers::PicoW::CYW43::Connect(
			network.SSID
		);
	}
	else
	{
		AL::Hardware::Drivers::PicoW::CYW43::Connect(
			WIFI_DEFAULT_SSID,
			WIFI_DEFAULT_PASSWORD,
			WIFI_DEFAULT_AUTH_TYPE
		);
	}
}

// @throw AL::Exception
void wifi_open()
{
	AL::OS::Console::WriteLine(
		"Opening AL::Hardware::Drivers::PicoW::CYW43"
	);

	AL::Hardware::Drivers::PicoW::CYW43::Open(
		AL::Hardware::Drivers::PicoW::CYW43Countries::USA
	);
}

void wifi_close()
{
	AL::OS::Console::WriteLine(
		"Closing AL::Hardware::Drivers::PicoW::CYW43"
	);

	AL::Hardware::Drivers::PicoW::CYW43::Close();
}

// @throw AL::Exception
// @return false to shutdown
bool do_the_thing_once(AL::TimeSpan delta)
{
	using namespace AL;
	using namespace AL::Network;
	using namespace AL::Network::HTTP;

	Request request(
		Versions::HTTP_1_1,
		RequestMethods::GET
	);

	auto uri = Uri::FromString(
		"http://192.168.0.102/"
	);

	auto response = request.Execute(
		uri
	);

	if (response.GetStatus() == StatusCodes::OK)
	{
		for (auto& responseHeader : response.GetHeader())
		{
			OS::Console::WriteLine(
				"%s: %s",
				responseHeader.Key.GetCString(),
				responseHeader.Value.GetCString()
			);
		}

		OS::Console::WriteLine(
			response.GetContent()
		);
	}

	return true;
}

// @throw AL::Exception
void do_the_thing()
{
	AL::OS::Pico::Stdio::Init();

	for (int i = 0; i < 5; ++i)
	{
		AL::OS::Console::WriteLine(
			"Starting in %i..",
			5 - i
		);

		AL::Sleep(
			AL::TimeSpan::FromSeconds(1)
		);

		AL::OS::Console::Clear();
	}

	wifi_open();

	AL::Hardware::Drivers::PicoW::CYW43Network network;
	AL::Bool                                   networkFound;

	wifi_scan(
		network,
		networkFound
	);

	wifi_connect(
		network,
		networkFound
	);

	AL::Hardware::Drivers::PicoW::CYW43::LED::Write(
		AL::True
	);

	for (AL::OS::Timer timer; do_the_thing_once(timer.GetElapsed()); timer.Reset())
	{
		AL::Sleep(
			AL::TimeSpan::FromSeconds(5)
		);
	}

	AL::Hardware::Drivers::PicoW::CYW43::LED::Write(
		AL::False
	);

	wifi_close();
}

int main()
{
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

	AL::Sleep(
		AL::TimeSpan::Infinite
	);

	return 0;
}
