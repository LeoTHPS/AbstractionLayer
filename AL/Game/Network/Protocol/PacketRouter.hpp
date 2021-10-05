#pragma once
#include "AL/Common.hpp"

#include "Packet.hpp"
#include "PacketHandler.hpp"

#include "AL/Collections/Dictionary.hpp"

namespace AL::Game::Network::Protocol
{
	template<typename T_OPCODE, Endians ENDIAN>
	class PacketRouter
	{
		typedef Protocol::PacketHandler<T_OPCODE, ENDIAN> _PacketHandler;

		Collections::Dictionary<T_OPCODE, _PacketHandler> handlers;

	public:
		typedef T_OPCODE                         OPCode;
		typedef Protocol::Packet<OPCode, ENDIAN> Packet;
		typedef _PacketHandler                   PacketHandler;

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

		Void SetHandler(OPCode opcode, PacketHandler&& handler)
		{
			handlers.Add(
				opcode,
				Move(handler)
			);
		}

		Void RemoveHandler(OPCode opcode)
		{
			handlers.Remove(
				opcode
			);
		}

		// @return False if no handler
		Bool Execute(Packet& packet) const
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
