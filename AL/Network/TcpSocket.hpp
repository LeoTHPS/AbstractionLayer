#pragma once
#include "AL/Common.hpp"

#include "ISocket.hpp"

#if defined(AL_PLATFORM_PICO_W)
	#include "LWIP.hpp"
#elif defined(AL_PLATFORM_PICO)
	#warning Platform not supported
#elif defined(AL_PLATFORM_LINUX)
	#include <fcntl.h>
	#include <unistd.h>

	#include <arpa/inet.h>

	#include <sys/types.h>
	#include <sys/socket.h>
#elif defined(AL_PLATFORM_WINDOWS)
	#include <mstcpip.h>
#endif

namespace AL::Network
{
	class TcpSocket
		: public ISocket
	{
		Bool            isOpen      = False;
		Bool            isBound     = False;
		Bool            isBlocking  = True;
		Bool            isConnected = False;
		Bool            isListening = False;

		SocketTypes     type;

		IPEndPoint      localEP;
		IPEndPoint      remoteEP;
		AddressFamilies addressFamily;

#if defined(AL_PLATFORM_PICO_W)
		LWIP::TcpSocket socket;
#elif defined(AL_PLATFORM_LINUX)
		int             socket;
#elif defined(AL_PLATFORM_WINDOWS)
		::SOCKET        socket;
#endif

	public:
#if defined(AL_PLATFORM_PICO_W)
		typedef typename LWIP::TcpSocket::Handle Handle;
#elif defined(AL_PLATFORM_LINUX)
		typedef int      Handle;
#elif defined(AL_PLATFORM_WINDOWS)
		typedef ::SOCKET Handle;
#else
		typedef Void*    Handle;
#endif

#if defined(AL_PLATFORM_PICO_W)
		static constexpr size_t BACKLOG_MAX = LWIP::TcpSocket::BACKLOG_MAX;
#elif defined(AL_PLATFORM_LINUX)
		static constexpr size_t BACKLOG_MAX = SOMAXCONN;
#elif defined(AL_PLATFORM_WINDOWS)
		static constexpr size_t BACKLOG_MAX = SOMAXCONN;
#endif

		TcpSocket(TcpSocket&& tcpSocket)
			: isOpen(
				tcpSocket.isOpen
			),
			isBound(
				tcpSocket.isBound
			),
			isConnected(
				tcpSocket.isConnected
			),
			isListening(
				tcpSocket.isListening
			),
			type(
				tcpSocket.type
			),
			localEP(
				Move(tcpSocket.localEP)
			),
			remoteEP(
				Move(tcpSocket.remoteEP)
			),
			addressFamily(
				tcpSocket.addressFamily
			),
			socket(
				Move(tcpSocket.socket)
			)
		{
			tcpSocket.isOpen      = False;
			tcpSocket.isBound     = False;
			tcpSocket.isBlocking  = True;
			tcpSocket.isConnected = False;
		}

		explicit TcpSocket(AddressFamilies addressFamily)
			: type(
				SocketTypes::TCP
			),
			addressFamily(
				addressFamily
			)
#if defined(AL_PLATFORM_PICO_W)
			,
			socket(
				addressFamily
			)
#endif
		{
		}

		virtual ~TcpSocket()
		{
			if (IsOpen())
			{

				Close();
			}
		}

		virtual Bool IsOpen() const override
		{
			return isOpen;
		}

		virtual Bool IsBound() const
		{
			return isBound;
		}

		virtual Bool IsBlocking() const
		{
			return isBlocking;
		}

		virtual Bool IsConnected() const
		{
			return isConnected;
		}

		virtual Bool IsListening() const
		{
			return isListening;
		}

		virtual SocketTypes GetType() const override
		{
			return SocketTypes::TCP;
		}

		virtual Handle GetHandle() const
		{
#if defined(AL_PLATFORM_PICO_W)
			return socket.GetHandle();
#elif defined(AL_PLATFORM_LINUX)
			return socket;
#elif defined(AL_PLATFORM_WINDOWS)
			return socket;
#else
			return nullptr;
#endif
		}

		virtual AddressFamilies GetAddressFamily() const
		{
			return addressFamily;
		}

		virtual const IPEndPoint& GetLocalEndPoint() const
		{
			return localEP;
		}

		virtual const IPEndPoint& GetRemoteEndPoint() const
		{
			return remoteEP;
		}

		// @throw AL::Exception
		virtual Void Open() override
		{
			AL_ASSERT(
				!IsOpen(),
				"TcpSocket already open"
			);

#if defined(AL_PLATFORM_PICO_W)
			try
			{
				socket.Open();
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error opening LWIP::TcpSocket"
				);
			}
#elif defined(AL_PLATFORM_LINUX)
			if ((socket = ::socket(static_cast<typename Get_Enum_Or_Integer_Base<AddressFamilies>::Type>(GetAddressFamily()), SOCK_STREAM, IPPROTO_TCP)) == -1)
			{

				throw SocketException(
					"socket"
				);
			}
#elif defined(AL_PLATFORM_WINDOWS)
			if ((socket = ::WSASocketW(static_cast<typename Get_Enum_Or_Integer_Base<AddressFamilies>::Type>(GetAddressFamily()), SOCK_STREAM, IPPROTO_TCP, nullptr, 0, 0)) == INVALID_SOCKET)
			{

				throw SocketException(
					"WSASocketW"
				);
			}
#else
			throw PlatformNotSupportedException();
#endif

			isOpen = True;

			try
			{
				SetBlocking(
					IsBlocking()
				);
			}
			catch (Exception& exception)
			{
				Close();

				throw Exception(
					Move(exception),
					"Error applying blocking state"
				);
			}
		}

		virtual Void Close() override
		{
			if (IsOpen())
			{
#if defined(AL_PLATFORM_PICO_W)
				socket.Close();
#elif defined(AL_PLATFORM_LINUX)
				::close(
					socket
				);
#elif defined(AL_PLATFORM_WINDOWS)
				::closesocket(
					socket
				);
#endif

				isOpen      = False;
				isBound     = False;
				isConnected = False;
				isListening = False;
			}
		}

		// @throw AL::Exception
		virtual Void Bind(const IPEndPoint& ep)
		{
			AL_ASSERT(
				IsOpen(),
				"TcpSocket not open"
			);

#if defined(AL_PLATFORM_PICO_W)
			try
			{
				socket.Bind(
					ep
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error binding LWIP::TcpSocket"
				);
			}
#elif defined(AL_PLATFORM_LINUX) || defined(AL_PLATFORM_WINDOWS)
			auto address = GetNativeSocketAddress(
				ep
			);

	#if defined(AL_PLATFORM_LINUX)
			if (::bind(GetHandle(), &address.Address.addr, address.Size) == -1)
			{

				throw SocketException(
					"bind"
				);
			}
	#elif defined(AL_PLATFORM_WINDOWS)
			if (::bind(GetHandle(), &address.Address.addr, address.Size) == SOCKET_ERROR)
			{

				throw SocketException(
					"bind"
				);
			}
	#endif
#else
			throw NotImplementedException();
#endif

			isBound = True;
		}

		// @throw AL::Exception
		virtual Void Listen(size_t backlog)
		{
			AL_ASSERT(
				IsOpen(),
				"TcpSocket not open"
			);

			AL_ASSERT(
				!IsListening(),
				"TcpSocket already listening"
			);

			if (backlog > BACKLOG_MAX)
			{

				backlog = BACKLOG_MAX;
			}

#if defined(AL_PLATFORM_PICO_W)
			try
			{
				socket.Listen(
					backlog
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error listening to LWIP::TcpSocket"
				);
			}
#elif defined(AL_PLATFORM_LINUX)
			if (::listen(GetHandle(), static_cast<int>(backlog & Integer<uint32>::SignedCastMask)) == -1)
			{

				throw SocketException(
					"listen"
				);
			}
#elif defined(AL_PLATFORM_WINDOWS)
			if (::listen(GetHandle(), static_cast<int>(backlog & Integer<uint32>::SignedCastMask)) == SOCKET_ERROR)
			{

				throw SocketException(
					"listen"
				);
			}
#else
			throw NotImplementedException();
#endif

			isListening = True;
		}

		// @throw AL::Exception
		// @return AL::False on timeout
		virtual Bool Accept(TcpSocket& socket)
		{
			AL_ASSERT(
				IsOpen(),
				"TcpSocket not open"
			);

			AL_ASSERT(
				IsListening(),
				"TcpSocket not listening"
			);

			TcpSocket _socket(
				GetAddressFamily()
			);

#if defined(AL_PLATFORM_PICO_W)
			try
			{
				if (!this->socket.Accept(_socket.socket))
				{

					return False;
				}
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error accepting LWIP::TcpSocket"
				);
			}
#elif defined(AL_PLATFORM_LINUX)
			if ((_socket.socket = ::accept(GetHandle(), nullptr, nullptr)) == -1)
			{
				auto errorCode = GetLastError();

				if ((errorCode == EAGAIN) || (errorCode == EWOULDBLOCK))
				{

					return False;
				}

				throw SocketException(
					"accept",
					errorCode
				);
			}
#elif defined(AL_PLATFORM_WINDOWS)
			if ((_socket.socket = ::WSAAccept(GetHandle(), nullptr, nullptr, nullptr, 0)) == INVALID_SOCKET)
			{
				auto lastError = GetLastError();

				if (lastError == WSAEWOULDBLOCK)
				{

					return False;
				}

				throw SocketException(
					"WSAAccept",
					lastError
				);
			}
#else
			throw NotImplementedException();
#endif

			_socket.isOpen      = True;
			_socket.isBound     = False;
			_socket.isConnected = True;
			_socket.isListening = False;

#if defined(AL_PLATFORM_PICO_W)
			_socket.localEP  = _socket.socket.GetLocalEndPoint();
			_socket.remoteEP = _socket.socket.GetRemoteEndPoint();
#elif defined(AL_PLATFORM_LINUX) || defined(AL_PLATFORM_WINDOWS)
			try
			{
				ISocket::GetLocalEndPoint(
					_socket.localEP,
					_socket.GetHandle(),
					_socket.GetType(),
					_socket.GetAddressFamily()
				);
			}
			catch (Exception& exception)
			{
				_socket.Close();

				throw Exception(
					Move(exception),
					"Error getting local end point"
				);
			}

			try
			{
				ISocket::GetRemoteEndPoint(
					_socket.remoteEP,
					_socket.GetHandle(),
					_socket.GetType(),
					_socket.GetAddressFamily()
				);
			}
			catch (Exception& exception)
			{
				_socket.Close();

				throw Exception(
					Move(exception),
					"Error getting remote end point"
				);
			}
#endif

			try
			{
				_socket.SetBlocking(
					IsBlocking()
				);
			}
			catch (Exception& exception)
			{
				_socket.Close();

				throw Exception(
					Move(exception),
					"Error inheriting blocking state"
				);
			}

			socket = Move(
				_socket
			);

			return True;
		}

		// @throw AL::Exception
		// @return AL::False on timeout
		virtual Bool Connect(const IPEndPoint& ep)
		{
			AL_ASSERT(
				IsOpen(),
				"TcpSocket not open"
			);

			AL_ASSERT(
				!IsConnected(),
				"TcpSocket already connected"
			);

#if defined(AL_PLATFORM_PICO_W)
			try
			{
				if (!socket.Connect(ep, TimeSpan::FromSeconds(5)))
				{

					return False;
				}
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error connecting LWIP::TcpSocket"
				);
			}

			localEP  = socket.GetLocalEndPoint();
			remoteEP = socket.GetRemoteEndPoint();
#elif defined(AL_PLATFORM_LINUX) || defined(AL_PLATFORM_WINDOWS)
			auto isBlocking = IsBlocking();

			if (!isBlocking)
			{
				try
				{
					SetBlocking(
						True
					);
				}
				catch (Exception& exception)
				{

					throw Exception(
						Move(exception),
						"Error setting blocking state"
					);
				}
			}

			auto address = GetNativeSocketAddress(
				ep
			);

	#if defined(AL_PLATFORM_LINUX)
			if (::connect(GetHandle(), &address.Address.addr, address.Size) == -1)
			{
				auto errorCode = GetLastError();

				switch (errorCode)
				{
					case ETIMEDOUT:
					case EHOSTDOWN:
					case ENETUNREACH:
					case EHOSTUNREACH:
						return False;
				}

				throw SocketException(
					"connect",
					errorCode
				);
			}
	#elif defined(AL_PLATFORM_WINDOWS)
			if (::connect(GetHandle(), &address.Address.addr, address.Size) == SOCKET_ERROR)
			{
				auto lastError = GetLastError();

				switch (lastError)
				{
					case WSAETIMEDOUT:
					case WSAECONNREFUSED:
						return False;
				}

				throw SocketException(
					"connect",
					lastError
				);
			}
	#endif

			isConnected = True;

			if (!isBlocking)
			{
				try
				{
					SetBlocking(
						False
					);
				}
				catch (Exception& exception)
				{

					throw Exception(
						Move(exception),
						"Error setting blocking state"
					);
				}
			}

			try
			{
				ISocket::GetLocalEndPoint(
					localEP,
					GetHandle(),
					GetType(),
					GetAddressFamily()
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error getting local end point"
				);
			}

			try
			{
				ISocket::GetRemoteEndPoint(
					remoteEP,
					GetHandle(),
					GetType(),
					GetAddressFamily()
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error getting remote end point"
				);
			}
#else
			throw NotImplementedException();
#endif

			return True;
		}

		// @throw AL::Exception
		virtual Void Shutdown(SocketShutdownTypes type)
		{
			if (IsOpen() && IsConnected())
			{
#if defined(AL_PLATFORM_PICO_W)
				try
				{
					socket.Shutdown(
						(type == SocketShutdownTypes::Read) || (type == SocketShutdownTypes::ReadWrite),
						(type == SocketShutdownTypes::Write)
					);
				}
				catch (Exception& exception)
				{

					throw Exception(
						Move(exception),
						"Error shutting down LWIP::TcpSocket"
					);
				}
#elif defined(AL_PLATFORM_LINUX)
				int shutdownType;

				switch (type)
				{
					case SocketShutdownTypes::Read:
						shutdownType = SHUT_RD;
						break;

					case SocketShutdownTypes::Write:
						shutdownType = SHUT_WR;
						break;

					case SocketShutdownTypes::ReadWrite:
						shutdownType = SHUT_RDWR;
						break;

					default:
						throw NotImplementedException();
				}

				if (::shutdown(socket, shutdownType) == -1)
				{

					throw SocketException(
						"shutdown"
					);
				}
#elif defined(AL_PLATFORM_WINDOWS)
				int shutdownType;

				switch (type)
				{
					case SocketShutdownTypes::Read:
						shutdownType = SD_RECEIVE;
						break;

					case SocketShutdownTypes::Write:
						shutdownType = SD_SEND;
						break;

					case SocketShutdownTypes::ReadWrite:
						shutdownType = SD_BOTH;
						break;

					default:
						throw NotImplementedException();
				}

				if (::shutdown(socket, shutdownType) == SOCKET_ERROR)
				{

					throw SocketException(
						"shutdown"
					);
				}
#else
				throw NotImplementedException();
#endif
			}
		}

		// @throw AL::Exception
		// @return AL::False on connection closed
		virtual Bool Send(const Void* lpBuffer, size_t size, size_t& numberOfBytesSent)
		{
			AL_ASSERT(
				IsOpen(),
				"TcpSocket not open"
			);

			AL_ASSERT(
				IsConnected(),
				"TcpSocket not connected"
			);

#if defined(AL_PLATFORM_PICO_W)
			try
			{
				if (!socket.Send(lpBuffer, size, numberOfBytesSent))
				{
					Close();

					return False;
				}
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error sending LWIP::TcpSocket"
				);
			}
#elif defined(AL_PLATFORM_LINUX)
			ssize_t _numberOfBytesSent;

			if ((_numberOfBytesSent = ::send(GetHandle(), lpBuffer, size, 0)) == -1)
			{
				auto errorCode = GetLastError();

				if ((errorCode == EAGAIN) || (errorCode == EWOULDBLOCK))
				{
					numberOfBytesSent = 0;

					return True;
				}

				Close();

				if ((errorCode == EHOSTDOWN) || (errorCode == ECONNRESET) || (errorCode == EHOSTUNREACH))
				{

					return False;
				}

				throw SocketException(
					"recv",
					errorCode
				);
			}
			else if (_numberOfBytesSent == 0)
			{
				Close();

				return False;
			}

			numberOfBytesSent = static_cast<size_t>(
				_numberOfBytesSent & Integer<ssize_t>::SignedCastMask
			);
#elif defined(AL_PLATFORM_WINDOWS)
			int _numberOfBytesSent;

			if ((_numberOfBytesSent = ::send(GetHandle(), reinterpret_cast<const char*>(lpBuffer), static_cast<int>(size & Integer<int>::SignedCastMask), 0)) == SOCKET_ERROR)
			{
				switch (auto errorCode = GetLastError())
				{
					case WSAEWOULDBLOCK:
						numberOfBytesSent = 0;
						return True;

					case WSAENETDOWN:
					case WSAENETRESET:
					case WSAETIMEDOUT:
					case WSAECONNRESET:
					case WSAECONNABORTED:
					case WSAEHOSTUNREACH:
						Close();
						return False;

					default:
						throw SocketException(
							"send",
							errorCode
						);
				}
			}
			else if (_numberOfBytesSent == 0)
			{
				Close();

				return False;
			}

			numberOfBytesSent = static_cast<size_t>(
				_numberOfBytesSent & Integer<int>::SignedCastMask
			);
#else
			throw NotImplementedException();
#endif

			return True;
		}

		// @throw AL::Exception
		// @return AL::False on connection closed
		virtual Bool Receive(Void* lpBuffer, size_t size, size_t& numberOfBytesReceived)
		{
			AL_ASSERT(
				IsOpen(),
				"TcpSocket not open"
			);

			AL_ASSERT(
				IsConnected(),
				"TcpSocket not connected"
			);

#if defined(AL_PLATFORM_PICO_W)
			try
			{
				if (IsBlocking())
				{
					if (!socket.Receive(lpBuffer, size, numberOfBytesReceived))
					{

						return False;
					}
				}
				else if (!socket.Receive(lpBuffer, size, numberOfBytesReceived, 0))
				{

					numberOfBytesReceived = 0;
				}
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error receiving LWIP::TcpSocket"
				);
			}
#elif defined(AL_PLATFORM_LINUX)
			ssize_t _numberOfBytesReceived;

			if ((_numberOfBytesReceived = ::recv(GetHandle(), lpBuffer, size, 0)) == -1)
			{
				auto errorCode = GetLastError();

				if ((errorCode == EAGAIN) || (errorCode == EWOULDBLOCK))
				{
					numberOfBytesReceived = 0;

					return True;
				}

				Close();

				if ((errorCode == EHOSTDOWN) || (errorCode == ECONNRESET) || (errorCode == EHOSTUNREACH))
				{

					return False;
				}

				throw SocketException(
					"recv",
					errorCode
				);
			}
			else if (_numberOfBytesReceived == 0)
			{
				Close();

				return False;
			}

			numberOfBytesReceived = static_cast<size_t>(
				_numberOfBytesReceived & Integer<ssize_t>::SignedCastMask
			);
#elif defined(AL_PLATFORM_WINDOWS)
			int _numberOfBytesReceived;

			if ((_numberOfBytesReceived = ::recv(GetHandle(), reinterpret_cast<char*>(lpBuffer), static_cast<int>(size & Integer<int>::SignedCastMask), 0)) == SOCKET_ERROR)
			{
				switch (auto errorCode = GetLastError())
				{
					case WSAEWOULDBLOCK:
						numberOfBytesReceived = 0;
						return True;

					case WSAENETDOWN:
					case WSAENETRESET:
					case WSAETIMEDOUT:
					case WSAECONNRESET:
					case WSAECONNABORTED:
						Close();
						return False;

					default:
						throw SocketException(
							"recv",
							errorCode
						);
				}
			}
			else if (_numberOfBytesReceived == 0)
			{
				Close();

				return False;
			}

			numberOfBytesReceived = static_cast<size_t>(
				_numberOfBytesReceived & Integer<int>::SignedCastMask
			);
#else
			throw NotImplementedException();
#endif

			return True;
		}

		// @throw AL::Exception
		virtual Void SetBlocking(Bool value)
		{
			if (IsOpen())
			{
#if defined(AL_PLATFORM_PICO_W)
				// do nothing
				// this has to be emulated on the Pi Pico W due to LWIP not supporting it
#elif defined(AL_PLATFORM_LINUX)
				int flags;

				if ((flags = ::fcntl(GetHandle(), F_GETFL, 0)) == -1)
				{

					throw OS::SystemException(
						"fcntl"
					);
				}

				if (::fcntl(GetHandle(), F_SETFL, (!value ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK))) == -1)
				{

					throw OS::SystemException(
						"fcntl"
					);
				}
#elif defined(AL_PLATFORM_WINDOWS)
				::u_long arg = value ? 0 : 1;

				if (::ioctlsocket(GetHandle(), FIONBIO, &arg) == SOCKET_ERROR)
				{

					throw SocketException(
						"ioctlsocket"
					);
				}
#else
				throw NotImplementedException();
#endif
			}

			isBlocking = value;
		}

		TcpSocket& operator = (TcpSocket&& tcpSocket)
		{
			Close();

			isOpen = tcpSocket.isOpen;
			tcpSocket.isOpen = False;

			isBound = tcpSocket.isBound;
			tcpSocket.isBound = False;

			isBlocking = tcpSocket.isBlocking;
			tcpSocket.isBlocking = True;

			isConnected = tcpSocket.isConnected;
			tcpSocket.isConnected = False;

			isListening = tcpSocket.isListening;
			tcpSocket.isListening = False;

			type = tcpSocket.type;

			localEP = Move(
				tcpSocket.localEP
			);

			remoteEP = Move(
				tcpSocket.remoteEP
			);

			addressFamily = tcpSocket.addressFamily;

			socket = Move(
				tcpSocket.socket
			);

			return *this;
		}
	};
}
