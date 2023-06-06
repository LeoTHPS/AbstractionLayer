#pragma once
#include "AL/Common.hpp"

#include "Packet.hpp"
#include "PacketHandler.hpp"

#include "AL/Collections/Dictionary.hpp"

namespace AL::Game::Network::Protocol
{
	template<typename T_OPCODE>
	class PacketRouter
	{
		typedef T_OPCODE                         _OPCode;
		typedef Protocol::Packet<_OPCode>        _Packet;
		typedef Protocol::PacketHandler<_OPCode> _PacketHandler;

		Collections::Dictionary<_OPCode, _PacketHandler> handlers;

	public:
		typedef _OPCode        OPCode;
		typedef _Packet        Packet;
		typedef _PacketHandler PacketHandler;

		PacketRouter()
		{
		}

		PacketRouter(PacketRouter&& packetRouter)
			: handlers(
				Move(packetRouter.handlers)
			)
		{
		}
		PacketRouter(const PacketRouter& packetRouter)
			: handlers(
				packetRouter.handlers
			)
		{
		}

		virtual ~PacketRouter()
		{
		}

		template<typename F>
		Void SetHandler(OPCode opcode, F&& handler)
		{
			PacketHandler _handler(
				Move(handler)
			);

			SetHandler(
				opcode,
				Move(_handler)
			);
		}
		Void SetHandler(OPCode opcode, PacketHandler&& handler)
		{
			handlers[opcode] = Move(
				handler
			);
		}

		Void RemoveHandler(OPCode opcode)
		{
			handlers.Remove(
				opcode
			);
		}

		Bool Execute(Packet& packet)
		{
			auto it = handlers.Find(
				packet.GetOPCode()
			);

			if (it == handlers.end())
			{

				return False;
			}

			it->Value(
				packet
			);

			return True;
		}

		PacketRouter& operator = (PacketRouter&& packetRouter)
		{
			handlers = Move(
				packetRouter.handlers
			);

			return *this;
		}
		PacketRouter& operator = (const PacketRouter& packetRouter)
		{
			handlers = packetRouter.handlers;

			return *this;
		}

		Bool operator == (const PacketRouter& packetRouter) const
		{
			if (handlers != packetRouter.handlers)
			{

				return False;
			}

			return True;
		}
		Bool operator != (const PacketRouter& packetRouter) const
		{
			if (operator==(packetRouter))
			{

				return False;
			}

			return True;
		}
	};
}
