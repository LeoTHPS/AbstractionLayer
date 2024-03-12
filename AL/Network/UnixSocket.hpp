#pragma once
#include "AL/Common.hpp"

#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
	#include "AL/OS/Windows/NamedPipe.hpp"
#else
	#error Platform not supported
#endif

#include "ISocket.hpp"
#include "IPEndPoint.hpp"

#include "AL/FileSystem/Path.hpp"

namespace AL::Network
{
	class TcpSocket;
	class UdpSocket;

	template<typename T_SOCKET>
	class UnixSocket
	{
		static_assert(
			Is_Base_Of<ISocket, T_SOCKET>::Value,
			"T_SOCKET must inherit ISocket"
		);

		static constexpr Bool IS_TCP_SOCKET = Is_Base_Of<TcpSocket, T_SOCKET>::Value;
		static constexpr Bool IS_UDP_SOCKET = Is_Base_Of<UdpSocket, T_SOCKET>::Value;

		Bool                   isOpen     = False;
		Bool                   isBlocking = False;

		FileSystem::Path       path;

#if defined(AL_PLATFORM_LINUX)
		T_SOCKET               socket;
		IPEndPoint             remoteEP;
#elif defined(AL_PLATFORM_WINDOWS)
		OS::Windows::NamedPipe pipe;
#endif

	public:
		static constexpr Bool IsTcp()
		{
			return IS_TCP_SOCKET;
		}

		static constexpr Bool IsUdp()
		{
			return IS_UDP_SOCKET;
		}

		template<typename ... TArgs>
		UnixSocket(FileSystem::Path&& path, TArgs ... args)
			: path(
				Move(path)
			)
#if defined(AL_PLATFORM_LINUX)
			,
			socket(
				AddressFamilies::Unix
			),
			remoteEP{
				.Host = IPAddress::FromString(GetPath().GetString()),
				.Port = 0
			}
#elif defined(AL_PLATFORM_WINDOWS)
			,
			pipe(
				this->path.GetString()
			)
#endif
		{
		}
		template<typename ... TArgs>
		UnixSocket(const FileSystem::Path& path, TArgs ... args)
			: UnixSocket(
				FileSystem::Path(path),
				Forward<TArgs>(args) ...
			)
		{
		}

		virtual ~UnixSocket()
		{
			if (IsOpen())
			{

				Close();
			}
		}

		Bool IsOpen() const
		{
			return isOpen;
		}

		Bool IsBlocking() const
		{
			return isBlocking;
		}

		auto& GetPath() const
		{
			return path;
		}

		// @throw AL::Exception
		virtual Void SetBlocking(Bool set = True)
		{
#if defined(AL_PLATFORM_LINUX)
			if (IsOpen())
			{

				socket.SetBlocking(
					set
				);
			}
#endif

			isBlocking = set;
		}

		// @throw AL::Exception
		// @return AL::False on connection failed
		virtual Bool Open()
		{
			AL_ASSERT(
				!IsOpen(),
				"UnixSocket already open"
			);

#if defined(AL_PLATFORM_LINUX)
			try
			{
				socket.Open();
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error opening T_SOCKET"
				);
			}

			if constexpr (IS_TCP_SOCKET)
			{
				try
				{
					if (!socket.Connect(remoteEP))
					{

						return False;
					}
				}
				catch (Exception& exception)
				{
					socket.Close();

					throw Exception(
						Move(exception),
						"Error connecting T_SOCKET to '%s'",
						GetPath().GetString().GetCString()
					);
				}
			}
			else if constexpr (IS_UDP_SOCKET)
			{
				try
				{
					socket.Bind(
						remoteEP
					);
				}
				catch (Exception& exception)
				{
					socket.Close();

					throw Exception(
						Move(exception),
						"Error binding T_SOCKET to '%s'",
						GetPath().GetString().GetCString()
					);
				}
			}
#elif defined(AL_PLATFORM_WINDOWS)
			try
			{
				if (!pipe.Connect())
				{

					return False;
				}
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error opening OS::Windows::NamedPipe"
				);
			}
#endif

			isOpen = True;

			return True;
		}

		virtual Void Close()
		{
			if (IsOpen())
			{
#if defined(AL_PLATFORM_LINUX)
				socket.Close();
#elif defined(AL_PLATFORM_WINDOWS)
				pipe.Close();
#endif

				isOpen = False;
			}
		}

		// @throw AL::Exception
		// @return AL::False on connection closed
		virtual Bool Send(const Void* lpBuffer, size_t size, size_t& numberOfBytesSent)
		{
			AL_ASSERT(
				IsOpen(),
				"UnixSocket not open"
			);

#if defined(AL_PLATFORM_LINUX)
			if constexpr (IsTcp())
			{
			}
			else if constexpr (IsUdp())
			{
			}

			// TODO: implement
			throw AL::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			if (!pipe.Write(lpBuffer, size, numberOfBytesSent))
			{

				return False;
			}
#endif

			return True;
		}

		// @throw AL::Exception
		// @return AL::False on connection closed
		virtual Bool Receive(Void* lpBuffer, size_t size, size_t& numberOfBytesReceived)
		{
			AL_ASSERT(
				IsOpen(),
				"UnixSocket not open"
			);

#if defined(AL_PLATFORM_LINUX)
			if constexpr (IsTcp())
			{
			}
			else if constexpr (IsUdp())
			{
			}

			// TODO: implement
			throw AL::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			do
			{
				if (!pipe.Read(lpBuffer, size, numberOfBytesReceived))
				{

					return False;
				}
			} while (IsBlocking() && (numberOfBytesReceived == 0));
#endif

			return True;
		}
	};
}
