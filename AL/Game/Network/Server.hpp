#pragma once
#include "AL/Common.hpp"

#include "Session.hpp"

#include "AL/Network/Socket.hpp"
#include "AL/Network/IPEndPoint.hpp"

#include "AL/Collections/LinkedList.hpp"

namespace AL::Game::Network
{
	template<typename T_PACKET_OPCODE, Endians PACKET_ENDIAN>
	class Server;
}
