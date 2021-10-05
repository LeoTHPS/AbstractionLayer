#pragma once
#include "AL/Common.hpp"

#include "AL/Game/Network/Client.hpp"

static void AL_Game_Network_Client()
{
	using namespace AL;
	using namespace AL::Game::Network;

	enum class OPCodes : uint8
	{
	};

	Client<OPCodes, Endians::Big> client(
		Network::AddressFamilies::IPv4,
		1024
	);
}
