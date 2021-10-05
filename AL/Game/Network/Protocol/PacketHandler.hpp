#pragma once
#include "AL/Common.hpp"

#include "Packet.hpp"

namespace AL::Game::Network::Protocol
{
	template<typename T_OPCODE, Endians ENDIAN>
	using PacketHandler = Function<Void(Packet<T_OPCODE, ENDIAN>& packet)>;
}
