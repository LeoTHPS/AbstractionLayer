#pragma once
#include "AL/Common.hpp"

namespace AL::Game::Network::Protocol
{
#pragma pack(push, 1)
	template<typename T_OPCODE>
	struct PacketHeader
	{
		T_OPCODE OPCode;
		uint16   DataSize;
		uint32   DataHash;
	};
#pragma pack(pop)
}
