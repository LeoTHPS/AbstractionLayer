#pragma once
#include "AL/Common.hpp"

#include "Socket.hpp"
#include "ServerSession.hpp"

#include "AL/Collections/LinkedList.hpp"

namespace AL::Game::Network
{
	// @throw AL::Exception
	typedef Function<Void()> ServerOnListenEventHandler;

	// @throw AL::Exception
	typedef Function<Void()> ServerOnShutdownEventHandler;

	// @throw AL::Exception
	typedef Function<Void(TimeSpan delta)> ServerOnUpdateEventHandler;

	// @throw AL::Exception
	// @return AL::False to reject connection
	typedef Function<Bool(Socket& socket)> ServerOnAcceptEventHandler;

	// @throw AL::Exception
	template<typename T_OPCODE>
	using ServerOnConnectedEventHandler = Function<Void(ServerSession<T_OPCODE>& session)>;

	// @throw AL::Exception
	template<typename T_OPCODE>
	using ServerOnDisconnectedEventHandler = Function<Void(ServerSession<T_OPCODE>& session)>;

	template<typename T_OPCODE>
	class Server
	{
		typedef T_OPCODE                                  _OPCode;
		typedef ServerSession<_OPCode>                    _ServerSession;
		typedef ServerOnConnectedEventHandler<_OPCode>    _ServerOnConnectedEventHandler;
		typedef ServerOnDisconnectedEventHandler<_OPCode> _ServerOnDisconnectedEventHandler;

		Bool                                     isUpdating = False;
		Bool                                     isShutdownPending = False;

		Socket*                                  lpSocket = nullptr;

		Collections::LinkedList<_ServerSession*> sessions;

		size_t                                   receiveBufferSize;
		size_t                                   packetBuilderBufferSize;

		Server(Server&&) = delete;
		Server(const Server&) = delete;

	public:
		typedef _ServerSession Session;

		// @throw AL::Exception
		Event<ServerOnListenEventHandler>        OnListen;
		// @throw AL::Exception
		Event<ServerOnShutdownEventHandler>      OnShutdown;

		// @throw AL::Exception
		Event<ServerOnUpdateEventHandler>        OnUpdate;

		// @throw AL::Exception
		// @return AL::False to reject connection
		Event<ServerOnAcceptEventHandler>        OnAccept;

		// @throw AL::Exception
		Event<_ServerOnConnectedEventHandler>    OnConnected;
		// @throw AL::Exception
		Event<_ServerOnDisconnectedEventHandler> OnDisconnected;

		Server(size_t receiveBufferSize, size_t packetBuilderBufferSize)
			: receiveBufferSize(
				receiveBufferSize
			),
			packetBuilderBufferSize(
				packetBuilderBufferSize
			)
		{
		}

		virtual ~Server()
		{
			if (IsListening())
			{

				Shutdown();
			}
		}

		Bool IsListening() const
		{
			return lpSocket != nullptr;
		}

		auto GetSessionCount() const
		{
			return sessions.GetSize();
		}

		// @throw AL::Exception
		Void Listen(const IPEndPoint& localEP, uint32 backlog)
		{
			AL_ASSERT(
				!IsListening(),
				"Server already listening"
			);

			lpSocket = new Socket(
				localEP.Host.GetFamily()
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
				lpSocket->Bind(
					localEP
				);
			}
			catch (Exception& exception)
			{
				lpSocket->Close();

				delete lpSocket;
				lpSocket = nullptr;

				throw Exception(
					Move(exception),
					"Error binding Socket [Host: %s, Port: %u]",
					localEP.Host.ToString().GetCString(),
					localEP.Port
				);
			}

			try
			{
				lpSocket->Listen(
					backlog
				);
			}
			catch (Exception& exception)
			{
				lpSocket->Close();

				delete lpSocket;
				lpSocket = nullptr;

				throw Exception(
					Move(exception),
					"Error putting Socket into listening mode"
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
					"Error putting Socket into non-blocking mode"
				);
			}

			try
			{
				OnListen.Execute();
			}
			catch (Exception& exception)
			{
				lpSocket->Close();

				delete lpSocket;
				lpSocket = nullptr;

				throw;
			}
		}

		Void Shutdown()
		{
			if (IsListening())
			{
				if (isUpdating)
				{
					isShutdownPending = True;

					return;
				}

				lpSocket->Close();

				for (auto it = sessions.begin(); it != sessions.end(); )
				{
					auto lpSession = *it;

					lpSession->Disconnect();

					sessions.Erase(
						it++
					);
				}

				OnShutdown.Execute();

				delete lpSocket;
				lpSocket = nullptr;
			}
		}

		// @throw AL::Exception
		Void Update(TimeSpan delta)
		{
			AL_ASSERT(
				IsListening(),
				"Server not listening"
			);

			isUpdating = True;

			Handle_OnUpdate(
				delta
			);

			OnUpdate.Execute(
				delta
			);

			isUpdating = False;

			if (isShutdownPending)
			{
				isShutdownPending = False;

				Shutdown();
			}
		}

	private:
		// @throw AL::Exception
		Void Handle_OnUpdate(TimeSpan delta)
		{
			try
			{
				Handle_OnUpdate_Accept(
					delta
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error accepting new Session(s)"
				);
			}

			try
			{
				Handle_OnUpdate_Sessions(
					delta
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error updating Session(s)"
				);
			}
		}

		// @throw AL::Exception
		Void Handle_OnUpdate_Accept(TimeSpan delta)
		{
			Socket socket(
				lpSocket->GetAddressFamily()
			);

			while (IsListening() && lpSocket->Accept(socket))
			{
				if (!OnAccept.Execute(socket))
				{
					socket.Close();

					break;
				}

				auto lpSession = new Session(
					*this,
					Move(socket),
					receiveBufferSize,
					packetBuilderBufferSize
				);

				try
				{
					OnConnected.Execute(
						*lpSession
					);
				}
				catch (Exception&)
				{
					lpSession->Disconnect();

					delete lpSession;

					throw;
				}

				sessions.PushBack(
					lpSession
				);
			}
		}

		// @throw AL::Exception
		Void Handle_OnUpdate_Sessions(TimeSpan delta)
		{
			for (auto it = sessions.begin(); it != sessions.end(); )
			{
				auto lpSession = *it;

				if (!lpSession->Update(delta))
				{
					OnDisconnected.Execute(
						*lpSession
					);

					delete lpSession;

					sessions.Erase(
						it++
					);

					continue;
				}

				++it;
			}
		}
	};
}
