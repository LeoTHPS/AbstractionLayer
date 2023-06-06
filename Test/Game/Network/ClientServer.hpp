#pragma once
#include <AL/Common.hpp>

#include <AL/OS/Thread.hpp>
#include <AL/OS/Console.hpp>

#include <AL/Game/Loop.hpp>

#include <AL/Game/Network/Client.hpp>
#include <AL/Game/Network/Server.hpp>

enum class AL_Game_Network_OPCodes : AL::uint8
{
	Foo, Bar
};

typedef AL::Game::Network::Client<AL_Game_Network_OPCodes> AL_Game_Network_Client;

typedef AL::Game::Network::Server<AL_Game_Network_OPCodes> AL_Game_Network_Server;
typedef typename AL_Game_Network_Server::Session           AL_Game_Network_ServerSession;

typedef typename AL_Game_Network_Client::Packet            AL_Game_Network_Packet;
typedef typename AL_Game_Network_Client::PacketHandler     AL_Game_Network_PacketHandler;

// @throw AL::Exception
static void AL_Game_Network_ClientServer()
{
	using namespace AL;
	using namespace AL::Game;
	using namespace AL::Game::Network;

	IPEndPoint ep
	{
		.Host = IPAddress::Loopback(),
		.Port = 10000
	};

	AL_Game_Network_Client client(
		0xFF,
		0xFFFF
	);

	AL_Game_Network_Server server(
		0xFF,
		0xFFFF
	);

	client.SetPacketHandler(
		AL_Game_Network_OPCodes::Bar,
		[&client, &server](AL_Game_Network_Packet& _packet)
		{
#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
			OS::Console::WriteLine(
				"[Client] Received AL_Game_Network_OPCodes::Bar"
			);
#endif

			client.Disconnect();
		}
	);

	client.OnConnected.Register(
		[&client]()
		{
#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
			OS::Console::WriteLine(
				"[Client] Connected to %s:%u",
				client.GetRemoteEndPoint().Host.ToString().GetCString(),
				client.GetRemoteEndPoint().Port
			);
#endif
		}
	);

	client.OnDisconnected.Register(
		[&client, &server]()
		{
#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
			OS::Console::WriteLine(
				"[Client] Disconnected - Stopping server"
			);
#endif

			server.Shutdown();
		}
	);

	client.OnSend.Register(
		[&client](const Void* _lpBuffer, AL::size_t _size)
		{
#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
			OS::Console::WriteLine(
				"[Client] Sent %llu bytes: %s",
				_size,
				HexConverter::Encode(_lpBuffer, _size).GetCString()
			);
#endif
		}
	);

	client.OnReceive.Register(
		[&client](const Void* _lpBuffer, AL::size_t _size)
		{
#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
			OS::Console::WriteLine(
				"[Client] Received %llu bytes: %s",
				_size,
				HexConverter::Encode(_lpBuffer, _size).GetCString()
			);
#endif
		}
	);

	server.OnListen.Register(
		[&server]()
		{
#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
			OS::Console::WriteLine(
				"[Server] Listening"
			);
#endif
		}
	);

	server.OnShutdown.Register(
		[&server]()
		{
#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
			OS::Console::WriteLine(
				"[Server] Shutdown"
			);
#endif
		}
	);

	server.OnAccept.Register(
		[&server](Socket& _socket)
		{
#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
			OS::Console::WriteLine(
				"[Server] Accepted connection from %s:%u",
				_socket.GetRemoteEndPoint().Host.ToString().GetCString(),
				_socket.GetRemoteEndPoint().Port
			);
#endif

			return True;
		}
	);

	server.OnConnected.Register(
		[&server](AL_Game_Network_ServerSession& _session)
		{
#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
			OS::Console::WriteLine(
				"[Server] Connection established with %s:%u",
				_session.GetRemoteEndPoint().Host.ToString().GetCString(),
				_session.GetRemoteEndPoint().Port
			);
#endif

			_session.OnConnected.Register(
				[&server, &_session]()
				{
#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
					OS::Console::WriteLine(
						"[Session] Connected to %s:%u",
						_session.GetRemoteEndPoint().Host.ToString().GetCString(),
						_session.GetRemoteEndPoint().Port
					);
#endif
				}
			);

			_session.OnDisconnected.Register(
				[&server, &_session]()
				{
#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
					OS::Console::WriteLine(
						"[Session] Connection lost to %s:%u",
						_session.GetRemoteEndPoint().Host.ToString().GetCString(),
						_session.GetRemoteEndPoint().Port
					);
#endif
				}
			);

			_session.OnSend.Register(
				[&server, &_session](const Void* __lpBuffer, AL::size_t __size)
				{
#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
					OS::Console::WriteLine(
						"[Session] Sent %llu bytes: %s",
						__size,
						HexConverter::Encode(__lpBuffer, __size).GetCString()
					);
#endif
				}
			);

			_session.OnReceive.Register(
				[&server, &_session](const Void* __lpBuffer, AL::size_t __size)
				{
#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
					OS::Console::WriteLine(
						"[Session] Received %llu bytes: %s",
						__size,
						HexConverter::Encode(__lpBuffer, __size).GetCString()
					);
#endif
				}
			);

			_session.SetPacketHandler(
				AL_Game_Network_OPCodes::Foo,
				[&server, &_session](AL_Game_Network_Packet& __packet)
				{
#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
					OS::Console::WriteLine(
						"[Session] Received AL_Game_Network_OPCodes::Foo"
					);
#endif

					AL_Game_Network_Packet packet(
						AL_Game_Network_OPCodes::Bar,
						0
					);

					packet.Finalize();

					_session.Send(
						packet
					);
				}
			);
		}
	);

	server.OnDisconnected.Register(
		[&server](AL_Game_Network_ServerSession& _session)
		{
#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
			OS::Console::WriteLine(
				"[Server] Connection closed to %s:%u",
				_session.GetRemoteEndPoint().Host.ToString().GetCString(),
				_session.GetRemoteEndPoint().Port
			);
#endif
		}
	);

	server.Listen(
		ep,
		3
	);

	if (!client.Connect(ep))
	{
		server.Shutdown();

#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
		OS::Console::WriteLine(
			"Error connecting to %s:%u",
			ep.Host.ToString().GetCString(),
			ep.Port
		);
#endif
	}
	else
	{
		AL_Game_Network_Packet packet(
			AL_Game_Network_OPCodes::Foo,
			3
		);

		packet.Write(uint8(0x01));
		packet.Write(uint8(0x02));
		packet.Write(uint8(0x03));

		packet.Finalize();

		client.Send(
			packet
		);
	}

	Loop::Run(
		20,
		[&client, &server](TimeSpan _delta)
		{
			if (client.IsConnected() && !client.Update(_delta))
			{
			}

			if (server.IsListening())
			{

				server.Update(
					_delta
				);
			}

			return server.IsListening();
		}
	);
}
