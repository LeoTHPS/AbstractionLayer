#pragma once
#include <AL/Common.hpp>

#include <AL/OS/Console.hpp>

#include <AL/Network/HTTP/Request.hpp>

// @throw AL::Exception
static void AL_Network_HTTP_Request()
{
	using namespace AL;
	using namespace AL::Network::HTTP;

	Request request(
		Versions::HTTP_1_1,
		RequestMethods::GET
	);

	auto uri = Uri::FromString(
		"http://127.0.0.1/"
	);

	auto response = request.Execute(
		uri
	);

#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
	OS::Console::WriteLine(
		"Status: %u",
		response.GetStatus()
	);

	switch (response.GetVersion())
	{
		case Versions::HTTP_1_0:
			OS::Console::WriteLine("Version: 1.0");
			break;

		case Versions::HTTP_1_1:
			OS::Console::WriteLine("Version: 1.1");
			break;
	}

	for (auto& pair : response.GetHeader())
	{
		OS::Console::WriteLine(
			"%s: %s",
			pair.Key.GetCString(),
			pair.Value.GetCString()
		);
	}

	OS::Console::WriteLine(
		"Content: %s",
		response.GetContent().GetCString()
	);
#endif
}
