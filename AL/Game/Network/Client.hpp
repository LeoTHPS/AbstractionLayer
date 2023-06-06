#pragma once
#include "AL/Common.hpp"

#include "Session.hpp"

namespace AL::Game::Network
{
	template<typename T_OPCODE>
	class Client
		: public Session<SessionTypes::Client, T_OPCODE>
	{
		typedef Session<SessionTypes::Client, T_OPCODE> _Session;

	public:
		typedef typename _Session::OPCode OPCode;
		typedef typename _Session::Packet Packet;

		Client(size_t receiveBufferSize, size_t packetBuilderBufferSize)
			: _Session(
				receiveBufferSize,
				packetBuilderBufferSize
			)
		{
		}
	};
}
