#pragma once
#include <AL/Common.hpp>

#include <AL/OS/Console.hpp>

#include <AL/Collections/Array.hpp>

#include <AL/Network/UdpSocket.hpp>

// @throw AL::Exception
static void AL_Network_UdpSocket()
{
	using namespace AL;
	using namespace AL::Network;

	UdpSocket socket(
		AddressFamilies::IPv4
	);

	socket.Open();

	socket.Bind(
		IPEndPoint
		{
			.Host = IPAddress::Any(),
			.Port = 10000
		}
	);

	IPEndPoint remoteEP;
	AL::size_t numberOfBytesReceived;

	Collections::Array<uint8> buffer(
		0xFFF
	);

	socket.Send(
		&buffer[0],
		buffer.GetCapacity(),
		IPEndPoint { .Host = IPAddress::Loopback(), .Port = 10000 }
	);

	socket.SetBlocking(
		False
	);

	while ((numberOfBytesReceived = socket.Receive(&buffer[0], buffer.GetCapacity(), remoteEP)) != 0)
	{
#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
		OS::Console::WriteLine(
			"Received %s bytes from %s:%u [%s]",
			ToString(numberOfBytesReceived).GetCString(),
			remoteEP.Host.ToString().GetCString(),
			remoteEP.Port,
			HexConverter::Encode(&buffer[0], numberOfBytesReceived).GetCString()
		);
#endif
	}

	socket.Close();
}
