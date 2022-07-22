#include <AL/Common.hpp>

#include <AL/OS/Timer.hpp>
#include <AL/OS/Console.hpp>

#include <AL/OS/Pico/Stdio.hpp>

#include <AL/Hardware/PicoW/CYW43.hpp>

#include <AL/Network/LWIP.hpp>

#include <AL/Network/HTTP/Request.hpp>

#define WIFI_DEFAULT_SSID       "WiFi"
#define WIFI_DEFAULT_PASSWORD   "passwd"
#define WIFI_DEFAULT_AUTH_TYPE  AL::Hardware::PicoW::CYW43AuthTypes::WPA2_AES

#define HTTP_REQUEST_REMOTE_URL "http://192.168.0.102/"

// @throw AL::Exception
void test_http_request()
{
	AL::Network::HTTP::Request request(
		AL::Network::HTTP::Versions::HTTP_1_1,
		AL::Network::HTTP::RequestMethods::GET
	);

	auto uri = AL::Network::HTTP::Uri::FromString(
		HTTP_REQUEST_REMOTE_URL
	);

	auto HTTP_Versions_ToString = [](AL::Network::HTTP::Versions _value)
	{
		switch (_value)
		{
			case AL::Network::HTTP::Versions::HTTP_1_0:
				return "1.0";

			case AL::Network::HTTP::Versions::HTTP_1_1:
				return "1.1";
		}

		return "Unknown";
	};

	auto HTTP_Methods_ToString = [](AL::Network::HTTP::RequestMethods _value)
	{
		switch (_value)
		{
			case AL::Network::HTTP::RequestMethods::GET:
				return AL_NETWORK_HTTP_REQUEST_METHOD_GET;

			case AL::Network::HTTP::RequestMethods::HEAD:
				return AL_NETWORK_HTTP_REQUEST_METHOD_HEAD;

			case AL::Network::HTTP::RequestMethods::POST:
				return AL_NETWORK_HTTP_REQUEST_METHOD_POST;

			case AL::Network::HTTP::RequestMethods::PUT:
				return AL_NETWORK_HTTP_REQUEST_METHOD_PUT;

			case AL::Network::HTTP::RequestMethods::DELETE:
				return AL_NETWORK_HTTP_REQUEST_METHOD_DELETE;

			case AL::Network::HTTP::RequestMethods::CONNECT:
				return AL_NETWORK_HTTP_REQUEST_METHOD_CONNECT;

			case AL::Network::HTTP::RequestMethods::OPTIONS:
				return AL_NETWORK_HTTP_REQUEST_METHOD_OPTIONS;

			case AL::Network::HTTP::RequestMethods::TRACE:
				return AL_NETWORK_HTTP_REQUEST_METHOD_TRACE;

			case AL::Network::HTTP::RequestMethods::PATCH:
				return AL_NETWORK_HTTP_REQUEST_METHOD_PATCH;
		}

		return "Unknown";
	};

	AL::OS::Console::WriteLine(
		"Executing AL::Network::HTTP::Request [Uri: %s, Method: %s, Version: %s]",
		uri.ToString().GetCString(),
		HTTP_Methods_ToString(request.GetMethod()),
		HTTP_Versions_ToString(request.GetVersion())
	);

	auto response = request.Execute(
		uri
	);

	auto responseStatus = response.GetStatus();

	AL::OS::Console::WriteLine(
		"AL::Network::HTTP::Response Status: %u",
		responseStatus
	);

	if (responseStatus == AL::Network::HTTP::StatusCodes::OK)
	{
		AL::OS::Console::WriteLine(
			response.GetContent()
		);
	}
}

// @throw AL::Exception
void wifi_scan(AL::Hardware::PicoW::CYW43Network& network, AL::Bool& networkFound)
{
	AL::OS::Console::WriteLine(
		"Scanning for open WiFi networks"
	);

	networkFound = AL::False;

	AL::Hardware::PicoW::CYW43ScanCallback onScan(
		[&network, &networkFound](const AL::Hardware::PicoW::CYW43Network& _network)
		{
			if (_network.AuthType == AL::Hardware::PicoW::CYW43AuthTypes::Open)
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

	AL::Hardware::PicoW::CYW43::Scan(
		onScan
	);
}

// @throw AL::Exception
void wifi_connect(const AL::Hardware::PicoW::CYW43Network& network, AL::Bool networkFound)
{
	AL::OS::Console::WriteLine(
		"Connecting to %s",
		networkFound ? network.SSID.GetCString() : WIFI_DEFAULT_SSID
	);

	if (networkFound)
	{
		AL::Hardware::PicoW::CYW43::Connect(
			network.SSID
		);
	}
	else
	{
		AL::Hardware::PicoW::CYW43::Connect(
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
		"Opening AL::Hardware::PicoW::CYW43"
	);

	AL::Hardware::PicoW::CYW43::Open(
		AL::Hardware::PicoW::CYW43Countries::USA
	);
}

void wifi_close()
{
	AL::OS::Console::WriteLine(
		"Closing AL::Hardware::PicoW::CYW43"
	);

	AL::Hardware::PicoW::CYW43::Close();
}

// @throw AL::Exception
bool update_the_thing(AL::TimeSpan delta)
{
	test_http_request();

	return true;
}

// @throw AL::Exception
void do_the_thing()
{
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

	AL::Hardware::PicoW::CYW43Network network;
	AL::Bool                          networkFound;

	wifi_scan(
		network,
		networkFound
	);

	wifi_connect(
		network,
		networkFound
	);

	AL::Hardware::PicoW::CYW43::LED::Write(
		AL::True
	);

	for (AL::OS::Timer timer; update_the_thing(timer.GetElapsed()); timer.Reset())
	{
		AL::Sleep(
			AL::TimeSpan::FromSeconds(5)
		);
	}

	AL::Hardware::PicoW::CYW43::LED::Write(
		AL::False
	);

	wifi_close();
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

	AL::Sleep(
		AL::TimeSpan::Infinite
	);

	return 0;
}
