#pragma once
#include "AL/Common.hpp"

#include "Session.hpp"

namespace AL::Game::Network
{
	template<typename T_PACKET_OPCODE, Endians PACKET_ENDIAN>
	class Client
		: public Session<T_PACKET_OPCODE, PACKET_ENDIAN>
	{
		typedef Session<T_PACKET_OPCODE, PACKET_ENDIAN> _Session;

	public:
		typedef typename _Session::Socket        Socket;

		typedef typename _Session::Packet        Packet;
		typedef typename _Session::PacketHandler PacketHandler;

		using _Session::Session;
	};
}
