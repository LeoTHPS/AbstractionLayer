#pragma once
#include "AL/Common.hpp"

#include "IPAddress.hpp"
#include "IPEndPoint.hpp"
#include "SocketException.hpp"

#include "AL/OS/ErrorCode.hpp"

#if defined(AL_PLATFORM_PICO)
	#error Platform not supported
#elif defined(AL_PLATFORM_LINUX)
	#include <fcntl.h>
	#include <unistd.h>

	#include <arpa/inet.h>

	#include <sys/types.h>
	#include <sys/socket.h>
#elif defined(AL_PLATFORM_WINDOWS)
	#include "WinSock.hpp"

	#include <mstcpip.h>
#endif

namespace AL::Network
{
	enum class SocketTypes
	{
		Raw      = SOCK_RAW,
		Stream   = SOCK_STREAM,
		DataGram = SOCK_DGRAM
	};

	enum class SocketProtocols
	{
		IP  = IPPROTO_IP,
		TCP = IPPROTO_TCP,
		UDP = IPPROTO_UDP
	};

	enum class ShutdownTypes
	{
#if defined(AL_PLATFORM_LINUX)
		Read      = SHUT_RD,
		Write     = SHUT_WR,
		ReadWrite = SHUT_RDWR
#elif defined(AL_PLATFORM_WINDOWS)
		Read      = SD_RECEIVE,
		Write     = SD_SEND,
		ReadWrite = SD_BOTH
#endif
	};

	constexpr uint32 BACKLOG_MAX = SOMAXCONN;

	class Socket
	{
#if defined(AL_PLATFORM_WINDOWS)
		template<typename F>
		static Bool WSALoadExtension(const Socket& s, ::GUID guid, F* lpFunction)
		{
			::DWORD nBytesReturned = 0;

			if (::WSAIoctl(s.socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), lpFunction, sizeof(F), &nBytesReturned, nullptr, nullptr) != 0)
			{

				return False;
			}

			if (nBytesReturned != sizeof(F))
			{

				return False;
			}

			return True;
		}

		static BOOL PASCAL FAR ConnectEx(const Socket& s, __in_bcount(namelen) const struct ::sockaddr FAR *name, int namelen, ::PVOID lpSendBuffer, ::DWORD dwSendDataLength, ::LPDWORD lpdwBytesSent, ::LPOVERLAPPED lpOverlapped)
		{
			::LPFN_CONNECTEX function;

			if (WSALoadExtension(s, WSAID_CONNECTEX, &function))
			{
				return function(
					s.socket,
					name,
					namelen,
					lpSendBuffer,
					dwSendDataLength,
					lpdwBytesSent,
					lpOverlapped
				);
			}

			return FALSE;
		}

		static BOOL PASCAL FAR AcceptEx(const Socket& sListenSocket, const Socket& sAcceptSocket, ::PVOID lpOutputBuffer, ::DWORD dwReceiveDataLength, ::DWORD dwLocalAddressLength, ::DWORD dwRemoteAddressLength, ::LPDWORD lpdwBytesReceived, ::LPOVERLAPPED lpOverlapped)
		{
			::LPFN_ACCEPTEX function;

			if (WSALoadExtension(sListenSocket, WSAID_ACCEPTEX, &function))
			{
				if (function(sListenSocket.socket, sAcceptSocket.socket, lpOutputBuffer, dwReceiveDataLength, dwLocalAddressLength, dwRemoteAddressLength, lpdwBytesReceived, lpOverlapped))
				{

					return TRUE;
				}
			}

			return FALSE;
		}

		static BOOL PASCAL FAR GetAcceptExSockaddrs(const Socket& s, ::PVOID lpOutputBuffer, ::DWORD dwReceiveDataLength, ::DWORD dwLocalAddressLength, ::DWORD dwRemoteAddressLength, struct ::sockaddr **LocalSockaddr, ::LPINT LocalSockaddrLength, struct ::sockaddr **RemoteSockaddr, ::LPINT RemoteSockaddrLength)
		{
			::LPFN_GETACCEPTEXSOCKADDRS function;

			if (WSALoadExtension(s, WSAID_GETACCEPTEXSOCKADDRS, &function))
			{
				function(
					lpOutputBuffer,
					dwReceiveDataLength,
					dwLocalAddressLength,
					dwRemoteAddressLength,
					LocalSockaddr,
					LocalSockaddrLength,
					RemoteSockaddr,
					RemoteSockaddrLength
				);

				return TRUE;
			}

			return FALSE;
		}

		static BOOL PASCAL FAR DisconnectEx(const Socket& s, ::LPOVERLAPPED lpOverlapped, ::DWORD  dwFlags, ::DWORD  dwReserved)
		{
			::LPFN_DISCONNECTEX function;

			if (WSALoadExtension(s, WSAID_DISCONNECTEX, &function))
			{
				if (function(s.socket, lpOverlapped, dwFlags, dwReserved))
				{

					return TRUE;
				}
			}

			return FALSE;
		}

		static BOOL PASCAL FAR TransmitFile(const Socket& s, ::HANDLE hFile, ::DWORD nNumberOfBytesToWrite, ::DWORD nNumberOfBytesPerSend, ::LPOVERLAPPED lpOverlapped, ::LPTRANSMIT_FILE_BUFFERS lpTransmitBuffers, ::DWORD dwReserved)
		{
			::LPFN_TRANSMITFILE function;

			if (WSALoadExtension(s, WSAID_TRANSMITFILE, &function))
			{
				if (function(s.socket, hFile, nNumberOfBytesToWrite, nNumberOfBytesPerSend, lpOverlapped, lpTransmitBuffers, dwReserved))
				{

					return TRUE;
				}
			}

			return FALSE;
		}
#endif

		Bool isOpen                         = False;
		Bool isBound                        = False;
		Bool isBlocking                     = True;
		Bool isConnected                    = False;
		Bool isListening                    = False;
		Bool isWinSockLoaded                = False;

#if defined(AL_PLATFORM_LINUX)
		int             socket;
#elif defined(AL_PLATFORM_WINDOWS)
		SOCKET          socket;
#endif
		SocketTypes     socketType;
		SocketProtocols socketProtocol;
		AddressFamilies addressFamily;
		IPEndPoint      localEndPoint;
		IPEndPoint      remoteEndPoint;

		Socket(const Socket&) = delete;

	public:
		static constexpr ssize_t WOULD_BLOCK       = -1;
		static constexpr ssize_t CONNECTION_CLOSED = 0;

		Socket(Socket&& socket)
			: isOpen(
				socket.isOpen
			),
			isBound(
				socket.isBound
			),
			isBlocking(
				socket.isBlocking
			),
			isConnected(
				socket.isConnected
			),
			isListening(
				socket.isListening
			),
			isWinSockLoaded(
				socket.isWinSockLoaded
			),
			socket(
				socket.socket
			),
			socketType(
				socket.socketType
			),
			socketProtocol(
				socket.socketProtocol
			),
			addressFamily(
				socket.addressFamily
			),
			localEndPoint(
				Move(socket.localEndPoint)
			),
			remoteEndPoint(
				Move(socket.remoteEndPoint)
			)
		{
			socket.isOpen = False;
			socket.isBound = False;
			socket.isBlocking = True;
			socket.isConnected = False;
			socket.isListening = False;
			socket.isWinSockLoaded = False;
		}

		Socket(SocketTypes type, SocketProtocols protocol, AddressFamilies addressFamily)
			: isWinSockLoaded(
#if defined(AL_PLATFORM_LINUX)
				False
#elif defined(AL_PLATFORM_WINDOWS)
				WinSock::TryLoad()
#endif
			),
			socketType(
				type
			),
			socketProtocol(
				protocol
			),
			addressFamily(
				addressFamily
			)
		{
		}

		virtual ~Socket()
		{
			if (IsOpen())
			{

				Close();
			}

#if defined(AL_PLATFORM_WINDOWS)
			if (isWinSockLoaded)
			{

				WinSock::Unload();
			}
#endif
		}

		Bool IsOpen() const
		{
			return isOpen;
		}

		Bool IsBound() const
		{
			return isBound;
		}

		Bool IsBlocking() const
		{
			return isBlocking;
		}

		Bool IsConnected() const
		{
			return isConnected;
		}

		Bool IsListening() const
		{
			return isListening;
		}

		auto GetType() const
		{
			return socketType;
		}

		auto GetHandle() const
		{
			return socket;
		}

		auto GetProtocol() const
		{
			return socketProtocol;
		}

		auto GetAddressFamily() const
		{
			return addressFamily;
		}

		auto& GetLocalEndPoint() const
		{
			return localEndPoint;
		}

		auto& GetRemoteEndPoint() const
		{
			return remoteEndPoint;
		}

		// @if IsOpen() throw AL::Exception
		Void SetBlocking(Bool set = True, Bool ignoreCurrentState = False)
		{
			if ((set != IsBlocking()) || ignoreCurrentState)
			{
				if (IsOpen())
				{
#if defined(AL_PLATFORM_LINUX)
					int flags;

					if ((flags = ::fcntl(GetHandle(), F_GETFL, 0)) == -1)
					{

						throw OS::SystemException(
							"fcntl"
						);
					}

					if (::fcntl(GetHandle(), F_SETFL, (!set ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK))) == -1)
					{

						throw OS::SystemException(
							"fcntl"
						);
					}
#elif defined(AL_PLATFORM_WINDOWS)
					::u_long arg = set ? 0 : 1;

					if (::ioctlsocket(GetHandle(), FIONBIO, &arg) == SOCKET_ERROR)
					{

						throw SocketException(
							"ioctlsocket"
						);
					}
#endif
				}

				isBlocking = set;
			}
		}

		// @throw AL::Exception
		Void Open()
		{
			AL_ASSERT(
				!IsOpen(),
				"Socket already open"
			);

#if defined(AL_PLATFORM_LINUX)
			if ((socket = ::socket(static_cast<int>(GetAddressFamily()), static_cast<int>(GetType()), static_cast<int>(GetProtocol()))) == -1)
			{

				throw SocketException(
					"socket"
				);
			}
#elif defined(AL_PLATFORM_WINDOWS)
			if ((socket = ::WSASocketW(static_cast<int>(GetAddressFamily()), static_cast<int>(GetType()), static_cast<int>(GetProtocol()), nullptr, 0, 0)) == INVALID_SOCKET)
			{

				throw SocketException(
					"WSASocketW"
				);
			}
#endif

			isOpen = True;

			localEndPoint = IPEndPoint();
			remoteEndPoint = IPEndPoint();

			try
			{
				SetBlocking(
					IsBlocking(),
					True
				);
			}
			catch (Exception& exception)
			{
				Close();

				throw Exception(
					Move(exception),
					"Error setting blocking state"
				);
			}
		}

		Void Close()
		{
			if (IsOpen())
			{
#if defined(AL_PLATFORM_LINUX)
				::close(
					GetHandle()
				);
#elif defined(AL_PLATFORM_WINDOWS)
				::closesocket(
					GetHandle()
				);
#endif

				isOpen = False;
				isBound = False;
				isBlocking = False;
				isConnected = False;
				isListening = False;
			}
		}

		// @throw AL::Exception
		Void Shutdown(ShutdownTypes type)
		{
			AL_ASSERT(
				IsOpen(),
				"Socket not open"
			);

#if defined(AL_PLATFORM_LINUX)
			if (::shutdown(GetHandle(), static_cast<int>(type)) == -1)
			{

				throw SocketException(
					"shutdown"
				);
			}
#elif defined(AL_PLATFORM_WINDOWS)
			if (::shutdown(GetHandle(), static_cast<int>(type)) == SOCKET_ERROR)
			{

				throw SocketException(
					"shutdown"
				);
			}
#endif
		}

		// @throw AL::Exception
		Void Bind(const IPEndPoint& ep)
		{
			AL_ASSERT(
				IsOpen(),
				"Socket not open"
			);

			AL_ASSERT(
				!IsBound(),
				"Socket already bound"
			);

			union
			{
				::sockaddr_in  v4;
				::sockaddr_in6 v6;
			} address;

			socklen_t addressSize;

			switch (GetAddressFamily())
			{
				case AddressFamilies::IPv4:
				{
					address.v4.sin_family = AF_INET;
					address.v4.sin_addr = ep.Host.GetAddress();
					address.v4.sin_port = BitConverter::HostToNetwork(
						ep.Port
					);

					addressSize = sizeof(::sockaddr_in);
				}
				break;

				case AddressFamilies::IPv6:
				{
					int v6_only = 0;

#if defined(AL_PLATFORM_LINUX)
					if (::setsockopt(GetHandle(), IPPROTO_IPV6, IPV6_V6ONLY, reinterpret_cast<const char*>(&v6_only), sizeof(v6_only)) == -1)
					{

						throw SocketException(
							"setsockopt"
						);
					}
#elif defined(AL_PLATFORM_WINDOWS)
					if (::setsockopt(GetHandle(), IPPROTO_IPV6, IPV6_V6ONLY, reinterpret_cast<const char*>(&v6_only), sizeof(v6_only)) == SOCKET_ERROR)
					{

						throw SocketException(
							"setsockopt"
						);
					}
#endif

					address.v6.sin6_family = AF_INET6;
					address.v6.sin6_addr = ep.Host.GetAddress6();
					address.v6.sin6_port = BitConverter::HostToNetwork(
						ep.Port
					);

					addressSize = sizeof(::sockaddr_in6);
				}
				break;

				default:
					throw NotImplementedException();
			}

#if defined(AL_PLATFORM_LINUX)
			if (::bind(GetHandle(), reinterpret_cast<const ::sockaddr*>(&address), addressSize) == -1)
			{

				throw SocketException(
					"bind"
				);
			}
#elif defined(AL_PLATFORM_WINDOWS)
			if (::bind(GetHandle(), reinterpret_cast<const ::sockaddr*>(&address), addressSize) == SOCKET_ERROR)
			{

				throw SocketException(
					"bind"
				);
			}
#endif

			isBound = True;
			localEndPoint = ep;
		}

		// @throw AL::Exception
		Void Listen(uint32 backlog = BACKLOG_MAX)
		{
			AL_ASSERT(
				IsOpen(),
				"Socket not open"
			);

			AL_ASSERT(
				IsBound(),
				"Socket not bound"
			);

			AL_ASSERT(
				!IsListening(),
				"Socket already listening"
			);

#if defined(AL_PLATFORM_LINUX)
			if (::listen(GetHandle(), static_cast<int32>(backlog & Integer<uint32>::SignedCastMask)) == -1)
			{

				throw SocketException(
					"listen"
				);
			}
#elif defined(AL_PLATFORM_WINDOWS)
			if (::listen(GetHandle(), static_cast<int32>(backlog & Integer<uint32>::SignedCastMask)) == SOCKET_ERROR)
			{

				throw SocketException(
					"listen"
				);
			}
#endif

			isListening = True;
		}

		// @throw AL::Exception
		// @return False if would block
		Bool Accept(Socket& socket) const
		{
			AL_ASSERT(
				IsOpen(),
				"Socket not open"
			);

			AL_ASSERT(
				IsListening(),
				"Socket not listening"
			);

			Socket _socket(
				GetType(),
				GetProtocol(),
				GetAddressFamily()
			);

#if defined(AL_PLATFORM_LINUX)
			if ((_socket.socket = ::accept(GetHandle(), nullptr, nullptr)) == -1)
			{
				auto errorCode = OS::GetLastError();

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
				auto lastError = OS::GetLastError();

				if (lastError == WSAEWOULDBLOCK)
				{

					return False;
				}

				throw SocketException(
					"WSAAccept",
					lastError
				);
			}
#endif

			_socket.isOpen = True;
			_socket.isConnected = True;

			try
			{
				GetLocalEndPoint(
					_socket.localEndPoint,
					_socket.socket,
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
				GetRemoteEndPoint(
					_socket.remoteEndPoint,
					_socket.socket,
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

#if defined(AL_PLATFORM_LINUX)
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
#elif defined(AL_PLATFORM_WINDOWS)
			_socket.isBlocking = IsBlocking();
#endif

			socket = Move(
				_socket
			);

			return True;
		}

		// @throw AL::Exception
		// @return False on timeout
		Bool Connect(const IPEndPoint& ep)
		{
			AL_ASSERT(
				IsOpen(),
				"Socket not open"
			);

			AL_ASSERT(
				!IsListening(),
				"Socket already listening"
			);

			AL_ASSERT(
				!IsConnected(),
				"Socket already connected"
			);

			auto addressFamily = GetAddressFamily();

			if (addressFamily != ep.Host.GetFamily())
			{

				throw Exception(
					"Address family mismatch"
				);
			}

			union
			{
				::sockaddr_in  v4;
				::sockaddr_in6 v6;
			} address;

			socklen_t addressSize;

			switch (addressFamily)
			{
				case AddressFamilies::IPv4:
				{
					address.v4.sin_family = AF_INET;
					address.v4.sin_addr = ep.Host.GetAddress();
					address.v4.sin_port = BitConverter::HostToNetwork(
						ep.Port
					);

					addressSize = sizeof(::sockaddr_in);
				}
				break;

				case AddressFamilies::IPv6:
				{
					address.v6.sin6_family = AF_INET6;
					address.v6.sin6_addr = ep.Host.GetAddress6();
					address.v6.sin6_port = BitConverter::HostToNetwork(
						ep.Port
					);

					addressSize = sizeof(::sockaddr_in6);
				}
				break;

				default:
					throw NotImplementedException();
			}

			auto isBlocking = IsBlocking();

			if (!isBlocking)
			{
				try
				{
					SetBlocking();
				}
				catch (Exception& exception)
				{

					throw Exception(
						Move(exception),
						"Error setting blocking state"
					);
				}
			}

#if defined(AL_PLATFORM_LINUX)
			if (::connect(GetHandle(), reinterpret_cast<const ::sockaddr*>(&address), addressSize) == -1)
			{
				auto errorCode = OS::GetLastError();

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
			if (::connect(GetHandle(), reinterpret_cast<const ::sockaddr*>(&address), addressSize) == SOCKET_ERROR)
			{
				auto lastError = OS::GetLastError();

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

			try
			{
				GetLocalEndPoint(
					localEndPoint,
					socket,
					GetAddressFamily()
				);
			}
			catch (Exception& exception)
			{
				Close();

				throw Exception(
					Move(exception),
					"Error getting local end point"
				);
			}

			try
			{
				GetRemoteEndPoint(
					remoteEndPoint,
					socket,
					GetAddressFamily()
				);
			}
			catch (Exception& exception)
			{
				Close();

				throw Exception(
					Move(exception),
					"Error getting remote end point"
				);
			}

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

			return True;
		}

		// @throw AL::Exception
		// @return WOULD_BLOCK on would block
		// @return CONNECTION_CLOSED on connection closed
		ssize_t Read(Void* lpBuffer, size_t size)
		{
			AL_ASSERT(
				IsOpen(),
				"Socket not open"
			);

			AL_ASSERT(
				IsConnected(),
				"Socket not connected"
			);

			if (size > Integer<ssize_t>::Maximum)
			{

				size = Integer<ssize_t>::Maximum;
			}

			ssize_t bytesReceived;

#if defined(AL_PLATFORM_LINUX)
			if ((bytesReceived = ::recv(GetHandle(), lpBuffer, size, 0)) == -1)
			{
				auto errorCode = OS::GetLastError();

				if ((errorCode == EAGAIN) || (errorCode == EWOULDBLOCK))
				{

					return WOULD_BLOCK;
				}

				Close();

				if ((errorCode == EHOSTDOWN) || (errorCode == ECONNRESET) || (errorCode == EHOSTUNREACH))
				{

					return CONNECTION_CLOSED;
				}

				throw SocketException(
					"recv",
					errorCode
				);
			}
			else if (bytesReceived == 0)
			{
				Close();

				return CONNECTION_CLOSED;
			}
#elif defined(AL_PLATFORM_WINDOWS)
			static constexpr int MAX_INT_VALUE = Integer<typename Get_Enum_Or_Integer_Base<int>::Type>::Maximum;

			if ((bytesReceived = ::recv(GetHandle(), reinterpret_cast<char*>(lpBuffer), static_cast<int>((size <= MAX_INT_VALUE) ? size : MAX_INT_VALUE), 0)) == SOCKET_ERROR)
			{
				switch (auto errorCode = OS::GetLastError())
				{
					case WSAEWOULDBLOCK:
						return WOULD_BLOCK;

					case WSAENETDOWN:
					case WSAENETRESET:
					case WSAETIMEDOUT:
					case WSAECONNRESET:
					case WSAECONNABORTED:
						Close();
						return CONNECTION_CLOSED;

					default:
						throw SocketException(
							"recv",
							errorCode
						);
				}
			}
			else if (bytesReceived == 0)
			{
				Close();

				return CONNECTION_CLOSED;
			}
#endif

			return bytesReceived;
		}

		// @throw AL::Exception
		// @return CONNECTION_CLOSED on connection closed
		ssize_t ReadAll(Void* lpBuffer, size_t size)
		{
			AL_ASSERT(
				IsOpen(),
				"Socket not open"
			);

			AL_ASSERT(
				IsConnected(),
				"Socket not connected"
			);

			if (size > Integer<ssize_t>::Maximum)
			{

				size = Integer<ssize_t>::Maximum;
			}

			size_t totalBytesReceived = 0;

			for (auto _lpBuffer = reinterpret_cast<uint8*>(lpBuffer); totalBytesReceived < size; )
			{
				switch (auto bytesReceived = Read(_lpBuffer, size - totalBytesReceived))
				{
					case WOULD_BLOCK:
						break;

					case CONNECTION_CLOSED:
						return CONNECTION_CLOSED;

					default:
						_lpBuffer += bytesReceived;
						totalBytesReceived += bytesReceived;
						break;
				}
			}

			return static_cast<ssize_t>(
				totalBytesReceived
			);
		}

		// @throw AL::Exception
		// @return WOULD_BLOCK on would block
		// @return CONNECTION_CLOSED on connection closed
		ssize_t TryReadAll(Void* lpBuffer, size_t size)
		{
			AL_ASSERT(
				IsOpen(),
				"Socket not open"
			);

			AL_ASSERT(
				IsConnected(),
				"Socket not connected"
			);

			if (size > Integer<ssize_t>::Maximum)
			{

				size = Integer<ssize_t>::Maximum;
			}

			auto bytesReceived = Write(
				lpBuffer,
				size
			);

			switch (bytesReceived)
			{
				case WOULD_BLOCK:
					return WOULD_BLOCK;

				case CONNECTION_CLOSED:
					return CONNECTION_CLOSED;
			}

			auto totalBytesReceived = static_cast<size_t>(
				bytesReceived
			);

			while (totalBytesReceived < size)
			{
				switch (bytesReceived = Read(&reinterpret_cast<uint8*>(lpBuffer)[totalBytesReceived], size - totalBytesReceived))
				{
					case WOULD_BLOCK:
						break;

					case CONNECTION_CLOSED:
						return CONNECTION_CLOSED;

					default:
						totalBytesReceived += bytesReceived;
						break;
				}
			}

			return static_cast<ssize_t>(
				totalBytesReceived
			);
		}

		// @throw AL::Exception
		// @return WOULD_BLOCK on would block
		// @return CONNECTION_CLOSED on connection closed
		ssize_t Write(const Void* lpBuffer, size_t size)
		{
			AL_ASSERT(
				IsOpen(),
				"Socket not open"
			);

			AL_ASSERT(
				IsConnected(),
				"Socket not connected"
			);

			if (size > Integer<ssize_t>::Maximum)
			{

				size = Integer<ssize_t>::Maximum;
			}

			ssize_t bytesSent;

#if defined(AL_PLATFORM_LINUX)
			if ((bytesSent = ::send(GetHandle(), lpBuffer, size, 0)) == -1)
			{
				auto errorCode = OS::GetLastError();

				if ((errorCode == EAGAIN) || (errorCode == EWOULDBLOCK))
				{

					return WOULD_BLOCK;
				}

				Close();

				if ((errorCode == EHOSTDOWN) || (errorCode == ECONNRESET) || (errorCode == EHOSTUNREACH))
				{

					return CONNECTION_CLOSED;
				}

				throw SocketException(
					"recv",
					errorCode
				);
			}
			else if (bytesSent == 0)
			{
				Close();

				return CONNECTION_CLOSED;
			}
#elif defined(AL_PLATFORM_WINDOWS)
			static constexpr int MAX_INT_VALUE = Integer<typename Get_Enum_Or_Integer_Base<int>::Type>::Maximum;

			if ((bytesSent = ::send(GetHandle(), reinterpret_cast<const char*>(lpBuffer), static_cast<int>((size <= MAX_INT_VALUE) ? size : MAX_INT_VALUE), 0)) == SOCKET_ERROR)
			{
				switch (auto errorCode = OS::GetLastError())
				{
					case WSAEWOULDBLOCK:
						return WOULD_BLOCK;

					case WSAENETDOWN:
					case WSAENETRESET:
					case WSAETIMEDOUT:
					case WSAECONNRESET:
					case WSAECONNABORTED:
					case WSAEHOSTUNREACH:
						Close();
						return CONNECTION_CLOSED;

					default:
						throw SocketException(
							"send",
							errorCode
						);
				}
			}
			else if (bytesSent == 0)
			{
				Close();

				return CONNECTION_CLOSED;
			}
#endif

			return bytesSent;
		}

		// @throw AL::Exception
		// @return CONNECTION_CLOSED on connection closed
		ssize_t WriteAll(const Void* lpBuffer, size_t size)
		{
			AL_ASSERT(
				IsOpen(),
				"Socket not open"
			);

			AL_ASSERT(
				IsConnected(),
				"Socket not connected"
			);

			if (size > Integer<ssize_t>::Maximum)
			{

				size = Integer<ssize_t>::Maximum;
			}

			size_t totalBytesSent = 0;

			for (auto _lpBuffer = reinterpret_cast<const uint8*>(lpBuffer); totalBytesSent < size; )
			{
				switch (auto bytesSent = Write(_lpBuffer, size - totalBytesSent))
				{
					case WOULD_BLOCK:
						break;

					case CONNECTION_CLOSED:
						return CONNECTION_CLOSED;

					default:
						_lpBuffer += bytesSent;
						totalBytesSent += bytesSent;
						break;
				}
			}

			return static_cast<ssize_t>(
				totalBytesSent
			);
		}

		// @throw AL::Exception
		// @return WOULD_BLOCK on would block
		// @return CONNECTION_CLOSED on connection closed
		ssize_t TryWriteAll(const Void* lpBuffer, size_t size)
		{
			AL_ASSERT(
				IsOpen(),
				"Socket not open"
			);

			AL_ASSERT(
				IsConnected(),
				"Socket not connected"
			);

			if (size > Integer<ssize_t>::Maximum)
			{

				size = Integer<ssize_t>::Maximum;
			}

			auto bytesSent = Write(
				lpBuffer,
				size
			);

			switch (bytesSent)
			{
				case WOULD_BLOCK:
					return WOULD_BLOCK;

				case CONNECTION_CLOSED:
					return CONNECTION_CLOSED;
			}

			auto totalBytesSent = static_cast<size_t>(
				bytesSent
			);

			while (totalBytesSent < size)
			{
				switch (bytesSent = Write(&reinterpret_cast<const uint8*>(lpBuffer)[totalBytesSent], size - totalBytesSent))
				{
					case WOULD_BLOCK:
						break;

					case CONNECTION_CLOSED:
						return CONNECTION_CLOSED;

					default:
						totalBytesSent += bytesSent;
						break;
				}
			}

			return static_cast<ssize_t>(
				totalBytesSent
			);
		}

		Socket& operator = (Socket&& socket)
		{
			if (IsOpen())
			{

				Close();
			}

			isOpen = socket.isOpen;
			socket.isOpen = False;

			isBound = socket.isBound;
			socket.isBound = False;

			isBlocking = socket.isBlocking;
			socket.isBlocking = True;

			isConnected = socket.isConnected;
			socket.isConnected = False;

			isListening = socket.isListening;
			socket.isListening = False;

			isWinSockLoaded = socket.isWinSockLoaded;
			socket.isWinSockLoaded = False;

			this->socket = socket.socket;

			socketType = socket.socketType;
			socketProtocol = socket.socketProtocol;
			addressFamily = socket.addressFamily;
			localEndPoint = Move(socket.localEndPoint);
			remoteEndPoint = Move(socket.remoteEndPoint);

			return *this;
		}

		Bool operator == (const Socket& socket) const
		{
			if (IsOpen() ^ socket.IsOpen())
			{

				return False;
			}

			if (IsOpen() && socket.IsOpen())
			{
				if (GetHandle() != socket.GetHandle())
				{

					return False;
				}
			}

			return True;
		}
		Bool operator != (const Socket& socket) const
		{
			if (operator==(socket))
			{

				return False;
			}

			return True;
		}

	private:
		// @throw AL::Exception
		static Void GetLocalEndPoint(IPEndPoint& ep, int socket, AddressFamilies addressFamily)
		{
			switch (addressFamily)
			{
				case AddressFamilies::IPv4:
				{
					sockaddr_in address;
					socklen_t   addressSize = sizeof(sockaddr_in);

#if defined(AL_PLATFORM_LINUX)
					if (::getsockname(socket, reinterpret_cast<sockaddr*>(&address), &addressSize) == -1)
					{

						throw SocketException(
							"getsockname"
						);
					}
#elif defined(AL_PLATFORM_WINDOWS)
					if (::getsockname(socket, reinterpret_cast<sockaddr*>(&address), &addressSize) == SOCKET_ERROR)
					{

						throw SocketException(
							"getsockname"
						);
					}
#endif

					ep.Host = Move(
						address.sin_addr
					);

					ep.Port = BitConverter::NetworkToHost(
						address.sin_port
					);
				}
				break;

				case AddressFamilies::IPv6:
				{
					sockaddr_in6 address;
					socklen_t    addressSize = sizeof(sockaddr_in6);

#if defined(AL_PLATFORM_LINUX)
					if (::getsockname(socket, reinterpret_cast<sockaddr*>(&address), &addressSize) == -1)
					{

						throw SocketException(
							"getsockname"
						);
					}
#elif defined(AL_PLATFORM_WINDOWS)
					if (::getsockname(socket, reinterpret_cast<sockaddr*>(&address), &addressSize) == SOCKET_ERROR)
					{

						throw SocketException(
							"getsockname"
						);
					}
#endif

					ep.Host = Move(
						address.sin6_addr
					);

					ep.Port = BitConverter::NetworkToHost(
						address.sin6_port
					);
				}
				break;

				default:
					throw NotImplementedException();
			}
		}

		// @throw AL::Exception
		static Void GetRemoteEndPoint(IPEndPoint& ep, int socket, AddressFamilies addressFamily)
		{
			switch (addressFamily)
			{
				case AddressFamilies::IPv4:
				{
					sockaddr_in address;
					socklen_t   addressSize = sizeof(sockaddr_in);

#if defined(AL_PLATFORM_LINUX)
					if (::getpeername(socket, reinterpret_cast<sockaddr*>(&address), &addressSize) == -1)
					{

						throw SocketException(
							"getpeername"
						);
					}
#elif defined(AL_PLATFORM_WINDOWS)
					if (::getpeername(socket, reinterpret_cast<sockaddr*>(&address), &addressSize) == SOCKET_ERROR)
					{

						throw SocketException(
							"getpeername"
						);
					}
#endif

					ep.Host = Move(
						address.sin_addr
					);

					ep.Port = BitConverter::NetworkToHost(
						address.sin_port
					);
				}
				break;

				case AddressFamilies::IPv6:
				{
					sockaddr_in6 address;
					socklen_t    addressSize = sizeof(sockaddr_in6);

#if defined(AL_PLATFORM_LINUX)
					if (::getpeername(socket, reinterpret_cast<sockaddr*>(&address), &addressSize) == -1)
					{

						throw SocketException(
							"getpeername"
						);
					}
#elif defined(AL_PLATFORM_WINDOWS)
					if (::getpeername(socket, reinterpret_cast<sockaddr*>(&address), &addressSize) == SOCKET_ERROR)
					{

						throw SocketException(
							"getpeername"
						);
					}
#endif

					ep.Host = Move(
						address.sin6_addr
					);

					ep.Port = BitConverter::NetworkToHost(
						address.sin6_port
					);
				}
				break;

				default:
					throw NotImplementedException();
			}
		}
	};
}
