#pragma once
#include "AL/Common.hpp"

#include "AL/Network/TcpSocket.hpp"
#include "AL/Network/SocketExtensions.hpp"

namespace AL::Game::Network
{
	typedef AL::Network::IPAddress           IPAddress;
	typedef AL::Network::IPEndPoint          IPEndPoint;

	typedef AL::Network::AddressFamilies     AddressFamilies;

	typedef AL::Network::TcpSocket           Socket;
	typedef AL::Network::SocketException     SocketException;
	typedef AL::Network::SocketExtensions    SocketExtensions;
	typedef AL::Network::SocketShutdownTypes SocketShutdownTypes;

#if defined(AL_PLATFORM_WINDOWS)
	typedef AL::Network::WinSock             WinSock;
#endif
}
