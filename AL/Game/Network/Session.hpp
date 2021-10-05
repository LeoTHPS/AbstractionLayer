#pragma once
#include "AL/Common.hpp"

#include "Protocol/Packet.hpp"
#include "Protocol/PacketRouter.hpp"
#include "Protocol/PacketBuilder.hpp"
#include "Protocol/PacketHandler.hpp"

#include "AL/Collections/Queue.hpp"

#include "AL/Network/Socket.hpp"
#include "AL/Network/IPEndPoint.hpp"

namespace AL::Game::Network
{
	template<typename T_PACKET_OPCODE, Endians PACKET_ENDIAN>
	class Session
	{
		typedef AL::Network::Socket                                     _Socket;

		typedef Protocol::Packet<T_PACKET_OPCODE, PACKET_ENDIAN>        _Packet;
		typedef Protocol::PacketRouter<T_PACKET_OPCODE, PACKET_ENDIAN>  _PacketRouter;
		typedef Protocol::PacketBuilder<T_PACKET_OPCODE, PACKET_ENDIAN> _PacketBuilder;
		typedef Protocol::PacketHandler<T_PACKET_OPCODE, PACKET_ENDIAN> _PacketHandler;

		static constexpr auto SOCKET_WOULD_BLOCK       = _Socket::WOULD_BLOCK;
		static constexpr auto SOCKET_CONNECTION_CLOSED = _Socket::CONNECTION_CLOSED;

		_Socket                     socket;

		Collections::Queue<_Packet> packetQueue;
		_PacketRouter               packetRouter;
		_PacketBuilder              packetBuilder;

		Session(Session&&) = delete;
		Session(const Session&) = delete;

	public:
		typedef _Socket        Socket;

		typedef _Packet        Packet;
		typedef _PacketHandler PacketHandler;

		Session(AL::Network::AddressFamilies addressFamily, size_t packetBufferSize)
			: socket(
				AL::Network::SocketTypes::Stream,
				AL::Network::SocketProtocols::TCP,
				addressFamily
			),
			packetBuilder(
				packetBufferSize
			)
		{
		}

		virtual ~Session()
		{
			if (IsConnected())
			{
				Disconnect();
			}
		}

		Bool IsConnected() const
		{
			return socket.IsConnected();
		}

		auto GetAddressFamily() const
		{
			return socket.GetAddressFamily();
		}

		auto& GetLocalEndPoint() const
		{
			return socket.GetLocalEndPoint();
		}

		auto& GetRemoteEndPoint() const
		{
			return socket.GetRemoteEndPoint();
		}

		// @throw AL::Exception
		// @return False on connection closed
		Bool Update(TimeSpan delta)
		{
			AL_ASSERT(
				IsConnected(),
				"Session not connected"
			);

			if (!OnUpdate(delta))
			{

				return False;
			}

			return True;
		}

		Void Disconnect()
		{
			if (IsConnected())
			{
				socket.Close();

				OnDisconnected();
			}
		}

		Void SendPacket(Packet&& packet)
		{
			PacketQueue_Push(
				Move(packet)
			);
		}

		// @throw AL::Exception
		Void SetSocket(Socket&& socket)
		{
			Bool wasConnected;

			if (wasConnected = IsConnected())
			{

				this->socket.Close();
			}

			this->socket = Move(
				socket
			);

			if (!wasConnected && this->socket.IsConnected())
			{

				OnConnected();
			}
			else if (wasConnected && !this->socket.IsConnected())
			{

				OnDisconnected();
			}
		}

		Void SetPacketHandler(typename Packet::OPCode opcode, PacketHandler&& handler)
		{
			packetRouter.SetHandler(
				opcode,
				Move(handler)
			);
		}

		Void RemovePacketHandler(typename Packet::OPCode opcode)
		{
			packetRouter.RemoveHandler(
				opcode
			);
		}

	protected:
		// @throw AL::Exception
		virtual Void OnConnected()
		{
		}

		virtual Void OnDisconnected()
		{
			packetBuilder.Clear();
		}

		// @throw AL::Exception
		// @return False on connection closed
		virtual Bool OnUpdate(TimeSpan delta)
		{
			if (!PacketQueue_Flush())
			{

				return False;
			}

			return True;
		}

		// @throw AL::Exception
		virtual Void OnRead(Packet& packet)
		{
		}
		// @throw AL::Exception
		virtual Void OnRead(Void* lpBuffer, size_t size)
		{
		}

		// @throw AL::Exception
		virtual Void OnWrite(const Packet& packet)
		{
		}
		// @throw AL::Exception
		virtual Void OnWrite(const Void* lpBuffer, size_t size)
		{
		}

	private:
		// @throw AL::Exception
		// @return False on connection closed
		Bool PacketQueue_Flush()
		{
			Packet packet;

			while (PacketQueue_Pop(packet))
			{
				if (!socket.IsConnected())
				{

					return False;
				}

				if (socket.WriteAll(packet.GetBuffer(), packet.GetBufferSize()) == SOCKET_CONNECTION_CLOSED)
				{

					return False;
				}
			}

			return True;
		}

		Bool PacketQueue_Pop(Packet& packet)
		{
			if (!packetQueue.Dequeue(packet))
			{

				return False;
			}

			return True;
		}

		Void PacketQueue_Push(Packet&& packet)
		{
			packetQueue.Enqueue(
				Move(packet)
			);
		}
	};
}
