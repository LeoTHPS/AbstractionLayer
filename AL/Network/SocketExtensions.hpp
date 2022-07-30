#pragma once
#include "AL/Common.hpp"

namespace AL::Network
{
	class  ISocket;
	class  TcpSocket;
	class  UdpSocket;
	struct IPEndPoint;

	class SocketExtensions
	{
		template<typename T_SOCKET>
		struct Is_Socket
		{
			static constexpr Bool Value = Is_Base_Of<ISocket, T_SOCKET>::Value;
		};

		template<typename T_SOCKET>
		struct Is_TcpSocket
		{
			static constexpr Bool Value = Is_Type<TcpSocket, T_SOCKET>::Value || Is_Base_Of<TcpSocket, T_SOCKET>::Value;
		};

		template<typename T_SOCKET>
		struct Is_UdpSocket
		{
			static constexpr Bool Value = Is_Type<UdpSocket, T_SOCKET>::Value || Is_Base_Of<UdpSocket, T_SOCKET>::Value;
		};

		SocketExtensions() = delete;

	public:
		// Send all bytes, waiting as needed
		// @throw AL::Exception
		// @return AL::False on connection closed
		template<typename T_SOCKET>
		static Bool SendAll(T_SOCKET& socket, const Void* lpBuffer, size_t size, size_t& numberOfBytesSent)
		{
			static_assert(
				Is_Socket<T_SOCKET>::Value,
				"T_SOCKET must be Socket"
			);

			numberOfBytesSent = 0;

			for (size_t _numberOfBytesSent = 0; numberOfBytesSent < size; )
			{
				if (!socket.Send(&reinterpret_cast<const uint8*>(lpBuffer)[numberOfBytesSent], size - numberOfBytesSent, _numberOfBytesSent))
				{

					return False;
				}

				numberOfBytesSent += _numberOfBytesSent;
			}

			return True;
		}

		// Send all bytes, bail if a wait is needed on the first try
		// @throw AL::Exception
		// @return AL::False on connection closed
		template<typename T_SOCKET>
		static Bool TrySendAll(T_SOCKET& socket, const Void* lpBuffer, size_t size, size_t& numberOfBytesSent)
		{
			static_assert(
				Is_Socket<T_SOCKET>::Value,
				"T_SOCKET must be Socket"
			);

			numberOfBytesSent = 0;

			for (size_t i = 0, _numberOfBytesSent = 0; numberOfBytesSent < size; ++i)
			{
				if (!socket.Send(&reinterpret_cast<const uint8*>(lpBuffer)[numberOfBytesSent], size - numberOfBytesSent, _numberOfBytesSent))
				{

					return False;
				}

				numberOfBytesSent += _numberOfBytesSent;

				if ((i == 0) && (numberOfBytesSent == 0))
				{

					break;
				}
			}

			return True;
		}

		// Send all bytes to an IPEndPoint, waiting as needed
		// @throw AL::Exception
		// @return AL::False on connection closed
		template<typename T_SOCKET>
		static Bool SendAllTo(T_SOCKET& socket, const Void* lpBuffer, size_t size, size_t& numberOfBytesSent, const IPEndPoint& ep)
		{
			static_assert(
				Is_Socket<T_SOCKET>::Value,
				"T_SOCKET must be Socket"
			);

			static_assert(
				Is_UdpSocket<T_SOCKET>::Value,
				"T_SOCKET must be UdpSocket"
			);

			numberOfBytesSent = 0;

			for (size_t _numberOfBytesSent = 0; numberOfBytesSent < size; )
			{
				if (!socket.SendTo(&reinterpret_cast<const uint8*>(lpBuffer)[numberOfBytesSent], size - numberOfBytesSent, _numberOfBytesSent, ep))
				{

					return False;
				}

				numberOfBytesSent += _numberOfBytesSent;
			}

			return True;
		}

		// Send all bytes to an IPEndPoint, bail if a wait is needed on the first try
		// @throw AL::Exception
		// @return AL::False on connection closed
		template<typename T_SOCKET>
		static Bool TrySendAllTo(T_SOCKET& socket, const Void* lpBuffer, size_t size, size_t& numberOfBytesSent, const IPEndPoint& ep)
		{
			static_assert(
				Is_Socket<T_SOCKET>::Value,
				"T_SOCKET must be Socket"
			);

			static_assert(
				Is_UdpSocket<T_SOCKET>::Value,
				"T_SOCKET must be UdpSocket"
			);

			numberOfBytesSent = 0;

			for (size_t i = 0, _numberOfBytesSent = 0; numberOfBytesSent < size; ++i)
			{
				if (!socket.SendTo(&reinterpret_cast<const uint8*>(lpBuffer)[numberOfBytesSent], size - numberOfBytesSent, _numberOfBytesSent, ep))
				{

					return False;
				}

				numberOfBytesSent += _numberOfBytesSent;

				if ((i == 0) && (numberOfBytesSent == 0))
				{

					break;
				}
			}

			return True;
		}

		// Receive all bytes, waiting as needed
		// @throw AL::Exception
		// @return AL::False on connection closed
		template<typename T_SOCKET>
		static Bool ReceiveAll(T_SOCKET& socket, Void* lpBuffer, size_t size, size_t& numberOfBytesReceived)
		{
			static_assert(
				Is_Socket<T_SOCKET>::Value,
				"T_SOCKET must be Socket"
			);

			numberOfBytesReceived = 0;

			for (size_t _numberOfBytesReceived = 0; numberOfBytesReceived < size; )
			{
				if (!socket.Receive(&reinterpret_cast<uint8*>(lpBuffer)[numberOfBytesReceived], size - numberOfBytesReceived, _numberOfBytesReceived))
				{

					return False;
				}

				numberOfBytesReceived += _numberOfBytesReceived;
			}

			return True;
		}

		// Receive all bytes, bail if a wait is needed on the first try
		// @throw AL::Exception
		// @return AL::False on connection closed
		template<typename T_SOCKET>
		static Bool TryReceiveAll(T_SOCKET& socket, Void* lpBuffer, size_t size, size_t& numberOfBytesReceived)
		{
			static_assert(
				Is_Socket<T_SOCKET>::Value,
				"T_SOCKET must be Socket"
			);

			numberOfBytesReceived = 0;

			for (size_t i = 0, _numberOfBytesReceived = 0; numberOfBytesReceived < size; ++i)
			{
				if (!socket.Receive(&reinterpret_cast<uint8*>(lpBuffer)[numberOfBytesReceived], size - numberOfBytesReceived, _numberOfBytesReceived))
				{

					return False;
				}

				numberOfBytesReceived += _numberOfBytesReceived;

				if ((i == 0) && (numberOfBytesReceived == 0))
				{

					break;
				}
			}

			return True;
		}
	};
}
