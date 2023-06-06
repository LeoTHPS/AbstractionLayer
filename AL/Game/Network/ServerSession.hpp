#pragma once
#include "AL/Common.hpp"

#include "Session.hpp"

namespace AL::Game::Network
{
	template<typename T_OPCODE>
	class Server;

	template<typename T_OPCODE>
	class ServerSession
		: public Session<SessionTypes::Server, T_OPCODE>
	{
		typedef Server<T_OPCODE>                        _Server;
		typedef Session<SessionTypes::Server, T_OPCODE> _Session;

		_Server* const lpServer;

	public:
		typedef typename _Session::OPCode OPCode;
		typedef typename _Session::Packet Packet;

		ServerSession(_Server& server, Socket&& socket, size_t receiveBufferSize, size_t packetBuilderBufferSize)
			: _Session(
				Move(socket),
				receiveBufferSize,
				packetBuilderBufferSize
			),
			lpServer(
				&server
			)
		{
		}

		auto& GetServer()
		{
			return *lpServer;
		}
		auto& GetServer() const
		{
			return *lpServer;
		}
	};
}
