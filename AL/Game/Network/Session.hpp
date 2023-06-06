#pragma once
#include "AL/Common.hpp"

#include "Socket.hpp"

#include "Protocol/Packet.hpp"
#include "Protocol/PacketRouter.hpp"
#include "Protocol/PacketBuilder.hpp"

#include "AL/Collections/Array.hpp"

namespace AL::Game::Network
{
	enum class SessionTypes : uint8
	{
		Client,
		Server
	};

	// @throw AL::Exception
	typedef EventHandler<Void(const Void* lpBuffer, size_t size)> SessionOnSendEventHandler;
	// @throw AL::Exception
	typedef EventHandler<Void(const Void* lpBuffer, size_t size)> SessionOnReceiveEventHandler;

	// @throw AL::Exception
	// @return AL::False on connection closed
	typedef EventHandler<Bool(TimeSpan delta)>                    SessionOnUpdateEventHandler;

	// @throw AL::Exception
	typedef EventHandler<Void()>                                  SessionOnConnectedEventHandler;
	typedef EventHandler<Void()>                                  SessionOnDisconnectedEventHandler;

	template<SessionTypes TYPE, typename T_OPCODE>
	class Session
	{
		typedef T_OPCODE                         _OPCode;
		typedef Protocol::Packet<_OPCode>        _Packet;
		typedef Protocol::PacketRouter<_OPCode>  _PacketRouter;
		typedef Protocol::PacketBuilder<_OPCode> _PacketBuilder;

		Socket*                   lpSocket = nullptr;

		Collections::Array<uint8> recvBuffer;

		_Packet                   packet;
		_PacketRouter             packetRouter;
		_PacketBuilder            packetBuilder;

		IPEndPoint                localEndPoint;
		IPEndPoint                remoteEndPoint;

		Session(Session&&) = delete;
		Session(const Session&) = delete;

	public:
		typedef _OPCode                               OPCode;
		typedef _Packet                               Packet;
		typedef typename _PacketRouter::PacketHandler PacketHandler;

		// @throw AL::Exception
		Event<SessionOnSendEventHandler>         OnSend;
		// @throw AL::Exception
		Event<SessionOnReceiveEventHandler>      OnReceive;

		// @throw AL::Exception
		// @return AL::False on connection closed
		Event<SessionOnUpdateEventHandler>       OnUpdate;

		// @throw AL::Exception
		Event<SessionOnConnectedEventHandler>    OnConnected;
		Event<SessionOnDisconnectedEventHandler> OnDisconnected;

		template<SessionTypes _TYPE = TYPE>
		Session(typename Enable_If<_TYPE == SessionTypes::Client, size_t>::Type receiveBufferSize, size_t packetBuilderBufferSize)
			: recvBuffer(
				receiveBufferSize
			),
			packetBuilder(
				packetBuilderBufferSize
			)
		{
		}

		template<SessionTypes _TYPE = TYPE>
		Session(Socket&& socket, typename Enable_If<_TYPE == SessionTypes::Server, size_t>::Type receiveBufferSize, size_t packetBuilderBufferSize)
			: lpSocket(
				new Socket(
					Move(socket)
				)
			),
			recvBuffer(
				receiveBufferSize
			),
			packetBuilder(
				packetBuilderBufferSize
			),
			localEndPoint(
				lpSocket->GetLocalEndPoint()
			),
			remoteEndPoint(
				lpSocket->GetRemoteEndPoint()
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
			return lpSocket != nullptr;
		}

		auto& GetLocalEndPoint() const
		{
			return localEndPoint;
		}

		auto& GetRemoteEndPoint() const
		{
			return remoteEndPoint;
		}

		// @throw AL::Exception
		// @return AL::False on timeout
		template<SessionTypes _TYPE = TYPE>
		typename Enable_If<_TYPE == SessionTypes::Client, Bool>::Type Connect(const IPEndPoint& remoteEP)
		{
			AL_ASSERT(
				!IsConnected(),
				"Session already connected"
			);

			lpSocket = new Socket(
				remoteEP.Host.GetFamily()
			);

			try
			{
				lpSocket->Open();
			}
			catch (Exception& exception)
			{
				delete lpSocket;
				lpSocket = nullptr;

				throw Exception(
					Move(exception),
					"Error opening Socket"
				);
			}

			try
			{
				if (!lpSocket->Connect(remoteEP))
				{
					delete lpSocket;
					lpSocket = nullptr;

					return False;
				}
			}
			catch (Exception& exception)
			{
				lpSocket->Close();

				delete lpSocket;
				lpSocket = nullptr;

				throw Exception(
					Move(exception),
					"Error establishing connection [Host: %s, Port: %u]",
					remoteEP.Host.ToString().GetCString(),
					remoteEP.Port
				);
			}

			try
			{
				lpSocket->SetBlocking(
					False
				);
			}
			catch (Exception& exception)
			{
				lpSocket->Close();

				delete lpSocket;
				lpSocket = nullptr;

				throw Exception(
					Move(exception),
					"Error setting non-blocking mode"
				);
			}

			localEndPoint  = lpSocket->GetLocalEndPoint();
			remoteEndPoint = lpSocket->GetRemoteEndPoint();

			try
			{
				OnConnected.Execute();
			}
			catch (Exception&)
			{
				lpSocket->Close();

				delete lpSocket;
				lpSocket = nullptr;

				throw;
			}

			return True;
		}

		Void Disconnect()
		{
			if (IsConnected())
			{
				lpSocket->Close();

				try
				{
					OnDisconnected.Execute();
				}
				catch (const Exception&)
				{
				}

				delete lpSocket;
				lpSocket = nullptr;
			}
		}

		// @throw AL::Exception
		// @return AL::False on connection closed
		Bool Send(const Packet& packet)
		{
			AL_ASSERT(
				packet.IsFinalized(),
				"Packet not finalized"
			);

			if (!IsConnected())
			{

				return False;
			}

			size_t numberOfBytesSent;

			if (!SocketExtensions::SendAll(*lpSocket, packet.GetBuffer(), packet.GetBufferSize(), numberOfBytesSent))
			{
				Disconnect();

				return False;
			}

			Handle_OnSend(
				packet.GetBuffer(),
				packet.GetBufferSize()
			);

			OnSend.Execute(
				packet.GetBuffer(),
				packet.GetBufferSize()
			);

			return True;
		}

		// @throw AL::Exception
		// @return AL::False on connection closed
		Bool Update(TimeSpan delta)
		{
			if (!IsConnected())
			{

				return False;
			}

			if (!Handle_OnUpdate(delta) ||
				!OnUpdate.Execute(delta))
			{
				if (IsConnected())
				{

					Disconnect();
				}

				return False;
			}

			return True;
		}

		template<typename F>
		Void SetPacketHandler(OPCode opcode, F&& handler)
		{
			packetRouter.SetHandler(
				opcode,
				Move(handler)
			);
		}
		Void SetPacketHandler(OPCode opcode, PacketHandler&& handler)
		{
			packetRouter.SetHandler(
				opcode,
				Move(handler)
			);
		}

	private:
		// @throw AL::Exception
		Void Handle_OnConnected()
		{
		}

		Void Handle_OnDisconnected()
		{
			packetBuilder.Clear();
		}

		// @throw AL::Exception
		Void Handle_OnSend(const Void* lpBuffer, size_t size)
		{
		}

		// @throw AL::Exception
		Void Handle_OnReceive(const Void* lpBuffer, size_t size)
		{
			if (packetBuilder.Append(lpBuffer, size))
			{
			}
		}

		// @throw AL::Exception
		// @return AL::False on connection closed
		Bool Handle_OnUpdate(TimeSpan delta)
		{
			try
			{
				if (!Handle_OnUpdate_Socket(delta))
				{

					return False;
				}
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error updating Socket"
				);
			}

			return True;
		}

		// @throw AL::Exception
		// @return AL::False on connection closed
		Bool Handle_OnUpdate_Socket(TimeSpan delta)
		{
			while (IsConnected())
			{
				size_t numberOfBytesReceived;

				if (!lpSocket->Receive(&recvBuffer[0], recvBuffer.GetCapacity(), numberOfBytesReceived))
				{

					return False;
				}

				if (numberOfBytesReceived == 0)
				{

					break;
				}

				Handle_OnReceive(
					&recvBuffer[0],
					static_cast<size_t>(numberOfBytesReceived)
				);

				OnReceive.Execute(
					&recvBuffer[0],
					static_cast<size_t>(numberOfBytesReceived)
				);

				while (packetBuilder.Dequeue(packet))
				{
					if (!packetRouter.Execute(packet))
					{
						Disconnect();

						break;
					}
				}
			}

			return True;
		}
	};
}
