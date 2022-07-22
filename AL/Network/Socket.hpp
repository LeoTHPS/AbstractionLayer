#pragma once
#include "AL/Common.hpp"

#include "IPAddress.hpp"
#include "IPEndPoint.hpp"

#include "ErrorCode.hpp"
#include "SocketException.hpp"

#include "AL/OS/Timer.hpp"

#if defined(AL_PLATFORM_PICO_W)
	#include "LWIP.hpp"

	#include <lwip/tcp.h>
	#include <lwip/udp.h>
	#include <lwip/pbuf.h>
#elif defined(AL_PLATFORM_PICO)
	#warning Platform not supported
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
#if defined(AL_PLATFORM_PICO_W)
		Stream,
		DataGram
#elif defined(AL_PLATFORM_LINUX)
		Raw      = SOCK_RAW,
		Stream   = SOCK_STREAM,
		DataGram = SOCK_DGRAM
#elif defined(AL_PLATFORM_WINDOWS)
		Raw      = SOCK_RAW,
		Stream   = SOCK_STREAM,
		DataGram = SOCK_DGRAM
#endif
	};

	enum class SocketProtocols
	{
#if defined(AL_PLATFORM_PICO_W)
		TCP = IP_PROTO_TCP,
		UDP = IP_PROTO_UDP
#elif defined(AL_PLATFORM_LINUX)
		IP  = IPPROTO_IP,
		TCP = IPPROTO_TCP,
		UDP = IPPROTO_UDP
#elif defined(AL_PLATFORM_WINDOWS)
		IP  = IPPROTO_IP,
		TCP = IPPROTO_TCP,
		UDP = IPPROTO_UDP
#endif
	};

	enum class ShutdownTypes
	{
#if defined(AL_PLATFORM_PICO_W)
		Read,
		Write,
		ReadWrite
#elif defined(AL_PLATFORM_LINUX)
		Read      = SHUT_RD,
		Write     = SHUT_WR,
		ReadWrite = SHUT_RDWR
#elif defined(AL_PLATFORM_WINDOWS)
		Read      = SD_RECEIVE,
		Write     = SD_SEND,
		ReadWrite = SD_BOTH
#endif
	};

#if defined(AL_PLATFORM_PICO_W)
	constexpr uint8  BACKLOG_MAX = 0xFF;
#elif defined(AL_PLATFORM_LINUX) || defined(AL_PLATFORM_WINDOWS)
	constexpr uint32 BACKLOG_MAX = SOMAXCONN;
#endif

	class Socket
	{
#if defined(AL_PLATFORM_PICO_W)
		enum class IOTypes : uint8
		{
			None,

			RX,
			TX,
			Accept,
			Connect
		};
#elif defined(AL_PLATFORM_WINDOWS)
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

		Bool isOpen          = False;
		Bool isBound         = False;
		Bool isBlocking      = True;
		Bool isConnected     = False;
		Bool isListening     = False;
		Bool isWinSockLoaded = False;

#if defined(AL_PLATFORM_PICO_W)
		union _Socket
		{
			::tcp_pcb* tcp;
			::udp_pcb* udp;
			Void*      handle;
		} socket;
#elif defined(AL_PLATFORM_LINUX)
		int             socket;
#elif defined(AL_PLATFORM_WINDOWS)
		::SOCKET        socket;
#endif
		SocketTypes     socketType;
		SocketProtocols socketProtocol;
		AddressFamilies addressFamily;
		IPEndPoint      localEndPoint;
		IPEndPoint      remoteEndPoint;

#if defined(AL_PLATFORM_PICO_W)
		struct
		{
			Bool      IsSuccess      = False;
			Bool      IsComplete     = False;
			Bool      IsTimedOut     = False;
			Bool      IsTimerEnabled = False;

			OS::Timer Timer;
			TimeSpan  Timeout;
			ErrorCode LastErrorCode    = 0;
			IOTypes   CurrentOperation = IOTypes::None;

			struct
			{
				Void*    lpBuffer;
				size_t   BufferSize;
				size_t   NumberOfBytesReceived;
			} RX;

			struct
			{
				const Void* lpBuffer;
				size_t      BufferSize;
				size_t      NumberOfBytesSent;
			} TX;

			struct
			{
			} Connect;
		} ioContext;
#endif

		Socket(const Socket&) = delete;

	public:
#if defined(AL_PLATFORM_PICO_W)
		typedef decltype(socket.handle) Handle;
#elif defined(AL_PLATFORM_LINUX)
		typedef decltype(socket)        Handle;
#elif defined(AL_PLATFORM_WINDOWS)
		typedef decltype(socket)        Handle;
#endif

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
#if defined(AL_PLATFORM_PICO_W)
			,
			ioContext(
				Move(socket.ioContext)
			)
#endif
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
#if defined(AL_PLATFORM_PICO_W)
				False
#elif defined(AL_PLATFORM_LINUX)
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
#if defined(AL_PLATFORM_PICO_W)
			return socket.handle;
#elif defined(AL_PLATFORM_LINUX) || defined(AL_PLATFORM_WINDOWS)
			return socket;
#endif
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
#if defined(AL_PLATFORM_PICO_W)
					// do nothing
					// non-blocking sockets aren't supported on this platform

					set = True;
#elif defined(AL_PLATFORM_LINUX)
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

#if defined(AL_PLATFORM_PICO_W)
			switch (GetType())
			{
				case SocketTypes::Stream:
				{
					if ((socket.tcp = ::tcp_new_ip_type(static_cast<typename Get_Enum_Or_Integer_Base<AddressFamilies>::Type>(GetAddressFamily()))) == nullptr)
					{

						throw SocketException(
							"tcp_new_ip_type",
							0
						);
					}

					LWIP_TCP_Init(
						socket.tcp
					);
				}
				break;

				case SocketTypes::DataGram:
				{
					if ((socket.udp = ::udp_new_ip_type(static_cast<typename Get_Enum_Or_Integer_Base<AddressFamilies>::Type>(GetAddressFamily()))) == nullptr)
					{

						throw SocketException(
							"udp_new_ip_type",
							0
						);
					}
				}
				break;

				default:
					throw NotImplementedException();
			}
#elif defined(AL_PLATFORM_LINUX)
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
#if defined(AL_PLATFORM_PICO_W)
				switch (GetType())
				{
					case SocketTypes::Stream:
						::tcp_close(socket.tcp);
						break;

					case SocketTypes::DataGram:
						::udp_disconnect(socket.udp);
						::udp_remove(socket.udp);
						break;
				}
#elif defined(AL_PLATFORM_LINUX)
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

#if defined(AL_PLATFORM_PICO_W)
			switch (GetType())
			{
				case SocketTypes::Stream:
				{
					ErrorCode errorCode;

					if ((errorCode = ::tcp_shutdown(socket.tcp, ((type == ShutdownTypes::Read) || (type == ShutdownTypes::ReadWrite)) ? 1 : 0, (type == ShutdownTypes::Write) ? 1 : 0)) != ::ERR_OK)
					{

						throw SocketException(
							"tcp_shutdown",
							errorCode
						);
					}
				}
				break;

				case SocketTypes::DataGram:
					break;

				default:
					throw NotImplementedException();
			}
#elif defined(AL_PLATFORM_LINUX)
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

#if defined(AL_PLATFORM_PICO_W)
			auto ip_addr = ep.Host.ToNative();

			switch (GetType())
			{
				case SocketTypes::Stream:
				{
					ErrorCode errorCode;

					if ((errorCode = ::tcp_bind(socket.tcp, &ip_addr, ep.Port)) != ::ERR_OK)
					{

						throw SocketException(
							"tcp_bind",
							errorCode
						);
					}
				}
				break;

				case SocketTypes::DataGram:
				{
					ErrorCode errorCode;

					if ((errorCode = ::udp_bind(socket.udp, &ip_addr, ep.Port)) != ::ERR_OK)
					{

						throw SocketException(
							"udp_bind",
							errorCode
						);
					}
				}
				break;

				default:
					throw NotImplementedException();
			}
#elif defined(AL_PLATFORM_LINUX) || defined(AL_PLATFORM_WINDOWS)
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

#if defined(AL_PLATFORM_PICO_W)
			switch (GetType())
			{
				case SocketTypes::Stream:
					// TODO: implement
					throw NotImplementedException();

				case SocketTypes::DataGram:
					throw OperationNotSupportedException();

				default:
					throw NotImplementedException();
			}
#elif defined(AL_PLATFORM_LINUX)
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
		// @return AL::False if would block
		Bool Accept(Socket& socket) const
		{
			if (!Accept(socket, TimeSpan::FromSeconds(5)))
			{

				return False;
			}

			return True;
		}
		// @throw AL::Exception
		// @return AL::False if would block
		Bool Accept(Socket& socket, TimeSpan timeout) const
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

#if defined(AL_PLATFORM_PICO_W)
			switch (GetType())
			{
				case SocketTypes::Stream:
					// TODO: implement
					throw NotImplementedException();

				case SocketTypes::DataGram:
					throw OperationNotSupportedException();

				default:
					throw NotImplementedException();
			}
#elif defined(AL_PLATFORM_LINUX)
			// TODO: implement timeout

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
			// TODO: implement timeout

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
#endif

			_socket.isOpen = True;
			_socket.isConnected = True;

			try
			{
				GetLocalEndPoint(
					_socket.localEndPoint,
					_socket.GetHandle(),
					GetType(),
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
					_socket.GetHandle(),
					GetType(),
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

#if defined(AL_PLATFORM_PICO_W) || defined(AL_PLATFORM_LINUX)
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
		// @return AL::False on timeout
		Bool Connect(const IPEndPoint& ep)
		{
			if (!Connect(ep, TimeSpan::FromSeconds(5)))
			{

				return False;
			}

			return True;
		}
		// @throw AL::Exception
		// @return AL::False on timeout
		Bool Connect(const IPEndPoint& ep, TimeSpan timeout)
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

#if defined(AL_PLATFORM_PICO_W)
			auto ip_addr = ep.Host.ToNative();

			switch (GetType())
			{
				case SocketTypes::Stream:
					if (!LWIP_TCP_Connect(ip_addr, ep.Port, timeout))
					{

						return False;
					}
					break;

				case SocketTypes::DataGram:
				{
					LWIP::Sync([this, &ip_addr, &ep]()
					{
						// TODO: refactor

						ErrorCode errorCode;

						if ((errorCode = ::udp_connect(socket.udp, &ip_addr, ep.Port)) != ::ERR_OK)
						{

							throw SocketException(
								"udp_connect",
								errorCode
							);
						}
					});
				}
				break;

				default:
					throw NotImplementedException();
			}
#elif defined(AL_PLATFORM_LINUX) || defined(AL_PLATFORM_WINDOWS)
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
			// TODO: implement timeout

			if (::connect(GetHandle(), reinterpret_cast<const ::sockaddr*>(&address), addressSize) == -1)
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
			// TODO: implement timeout

			if (::connect(GetHandle(), reinterpret_cast<const ::sockaddr*>(&address), addressSize) == SOCKET_ERROR)
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
#endif

			isConnected = True;

			try
			{
				GetLocalEndPoint(
					localEndPoint,
					GetHandle(),
					GetType(),
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
					GetHandle(),
					GetType(),
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

#if defined(AL_PLATFORM_PICO_W)
			switch (GetType())
			{
				case SocketTypes::Stream:
				{
					size_t numberOfBytesReceived;

					if ((bytesReceived = LWIP_TCP_Read(lpBuffer, size, numberOfBytesReceived)) == 0)
					{

						return CONNECTION_CLOSED;
					}

					bytesReceived = static_cast<ssize_t>(
						numberOfBytesReceived
					);
				}
				break;

				case SocketTypes::DataGram:
					// TODO: implement
					throw NotImplementedException();

				default:
					throw NotImplementedException();
			}
#elif defined(AL_PLATFORM_LINUX)
			if ((bytesReceived = ::recv(GetHandle(), lpBuffer, size, 0)) == -1)
			{
				auto errorCode = GetLastError();

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
				switch (auto errorCode = GetLastError())
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

#if defined(AL_PLATFORM_PICO_W)
			switch (GetType())
			{
				case SocketTypes::Stream:
					if ((bytesSent = LWIP_TCP_Write(lpBuffer, size)) == 0)
					{

						return CONNECTION_CLOSED;
					}
					break;

				case SocketTypes::DataGram:
					// TODO: implement
					throw NotImplementedException();

				default:
					throw NotImplementedException();
			}
#elif defined(AL_PLATFORM_LINUX)
			if ((bytesSent = ::send(GetHandle(), lpBuffer, size, 0)) == -1)
			{
				auto errorCode = GetLastError();

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
				switch (auto errorCode = GetLastError())
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
#if defined(AL_PLATFORM_PICO_W)
		Bool LWIP_IsOperationSuccess() const
		{
			return ioContext.IsSuccess;
		}

		Bool LWIP_IsOperationComplete() const
		{
			return ioContext.IsComplete;
		}

		Bool LWIP_IsOperationTimedOut() const
		{
			return ioContext.IsTimedOut;
		}

		Bool LWIP_IsOperationTimerEnabled() const
		{
			return ioContext.IsTimerEnabled;
		}

		Bool LWIP_IsOperationInProgress() const
		{
			return ioContext.CurrentOperation != IOTypes::None;
		}

		size_t LWIP_GetNumberOfBytesSent() const
		{
			return ioContext.TX.NumberOfBytesSent;
		}

		size_t LWIP_GetNumberOfBytesReceived() const
		{
			return ioContext.RX.NumberOfBytesReceived;
		}

		ErrorCode LWIP_GetLastOperationErrorCode() const
		{
			return ioContext.LastErrorCode;
		}

		Void LWIP_BeginOperation(IOTypes type)
		{
			LWIP_WaitForOperation();

			ioContext.CurrentOperation = type;
		}
		Void LWIP_BeginOperation(IOTypes type, TimeSpan timeout)
		{
			LWIP_WaitForOperation();

			ioContext.Timeout          = ioContext.Timer.GetElapsed() + timeout;
			ioContext.IsTimerEnabled   = True;
			ioContext.CurrentOperation = type;
		}

		// @return AL::True on success
		Bool LWIP_WaitForOperation() const
		{
			while (!LWIP_IsOperationComplete())
			{
				LWIP::Poll();
			}

			return LWIP_IsOperationSuccess();
		}

		// called by *OnComplete handlers
		Void LWIP_CompleteOperation(Bool isSuccess, Bool isTimedOut, ErrorCode errorCode)
		{
			ioContext.IsSuccess        = isSuccess;
			ioContext.IsTimedOut       = isTimedOut;
			ioContext.IsTimerEnabled   = False;
			ioContext.LastErrorCode    = errorCode;
			ioContext.CurrentOperation = IOTypes::None;
			ioContext.IsComplete       = True;
		}

		Void LWIP_TCP_Init(::tcp_pcb* lpPCB)
		{
			::tcp_arg(lpPCB, this);
			::tcp_err(lpPCB, &Socket::LWIP_TCP_OnError);
			::tcp_poll(lpPCB, &Socket::LWIP_TCP_OnPoll, 1);
			::tcp_recv(lpPCB, &Socket::LWIP_TCP_OnRead);
			::tcp_sent(lpPCB, &Socket::LWIP_TCP_OnWrite);
		}

		// @return AL::False on connection closed
		Bool LWIP_TCP_Read(Void* lpBuffer, size_t size, size_t& numberOfBytesReceived)
		{
			if (size > Integer<::u16_t>::Maximum)
			{

				size = Integer<::u16_t>::Maximum;
			}

			LWIP_WaitForOperation();

			ioContext.RX.lpBuffer              = lpBuffer;
			ioContext.RX.BufferSize            = size;
			ioContext.RX.NumberOfBytesReceived = 0;

			LWIP_BeginOperation(
				IOTypes::RX
			);

			if (!LWIP_WaitForOperation())
			{

				throw SocketException(
					"tcp_recv",
					LWIP_GetLastOperationErrorCode()
				);
			}

			if (LWIP_IsOperationTimedOut())
			{
				numberOfBytesReceived = 0;

				return True;
			}

			numberOfBytesReceived = LWIP_GetNumberOfBytesReceived();

			return True;
		}

		// @return 0 on connection closed
		// @return number of bytes written
		size_t LWIP_TCP_Write(const Void* lpBuffer, size_t size)
		{
			if (size > Integer<::u16_t>::Maximum)
			{

				size = Integer<::u16_t>::Maximum;
			}

			LWIP_BeginOperation(
				IOTypes::TX
			);

			LWIP::Sync([this, lpBuffer, size]()
			{
				ErrorCode errorCode;

				if ((errorCode = ::tcp_write(socket.tcp, lpBuffer, static_cast<::u16_t>(size), TCP_WRITE_FLAG_COPY)) != ::ERR_OK)
				{

					throw SocketException(
						"tcp_write",
						errorCode
					);
				}
			});

			if (!LWIP_WaitForOperation())
			{

				throw SocketException(
					"tcp_write",
					LWIP_GetLastOperationErrorCode()
				);
			}

			return LWIP_GetNumberOfBytesSent();
		}

		// @return AL::False on timeout
		Bool LWIP_TCP_Connect(const typename IPAddress::ip_addr& address, uint16 port, TimeSpan timeout)
		{
			LWIP_BeginOperation(
				IOTypes::Connect,
				timeout
			);

			LWIP::Sync([this, &address, port]()
			{
				ErrorCode errorCode;

				if ((errorCode = ::tcp_connect(socket.tcp, &address, port, &Socket::LWIP_TCP_OnConnect)) != ::ERR_OK)
				{

					throw SocketException(
						"tcp_connect",
						errorCode
					);
				}
			});

			if (!LWIP_WaitForOperation())
			{

				throw SocketException(
					"tcp_connect",
					LWIP_GetLastOperationErrorCode()
				);
			}

			if (LWIP_IsOperationTimedOut())
			{

				return False;
			}

			return True;
		}

		static ::err_t LWIP_TCP_OnPoll(void* lpParam, ::tcp_pcb* lpPCB)
		{
			auto lpSocket = reinterpret_cast<Socket*>(
				lpParam
			);

			return lpSocket->LWIP_TCP_OnPollComplete();
		}
		       ::err_t LWIP_TCP_OnPollComplete()
		{
			if (LWIP_IsOperationInProgress() && LWIP_IsOperationTimerEnabled())
			{
				if (ioContext.Timer.GetElapsed() >= ioContext.Timeout)
				{
					LWIP_CompleteOperation(
						True,
						True,
						::ERR_TIMEOUT
					);
				}
			}

			return ::ERR_OK;
		}

		static void    LWIP_TCP_OnError(void* lpParam, ::err_t errorCode)
		{
			auto lpSocket = reinterpret_cast<Socket*>(
				lpParam
			);

			return lpSocket->LWIP_TCP_OnErrorComplete(
				errorCode
			);
		}
		       Void    LWIP_TCP_OnErrorComplete(::err_t errorCode)
		{
			// TODO: implement
		}

		static ::err_t LWIP_TCP_OnConnect(void* lpParam, ::tcp_pcb* lpPCB, ::err_t errorCode)
		{
			auto lpSocket = reinterpret_cast<Socket*>(
				lpParam
			);

			return lpSocket->LWIP_TCP_OnConnectComplete(
				lpPCB,
				errorCode
			);
		}
		       ::err_t LWIP_TCP_OnConnectComplete(::tcp_pcb* lpPCB, ::err_t errorCode)
		{
			if ((errorCode == ::ERR_VAL) || (lpPCB == nullptr))
			{
				LWIP_CompleteOperation(
					False,
					False,
					errorCode
				);

				return errorCode;
			}

			socket.tcp = lpPCB;

			LWIP_CompleteOperation(
				True,
				False,
				::ERR_OK
			);

			return ::ERR_OK;
		}

		static ::err_t LWIP_TCP_OnRead(void* lpParam, ::tcp_pcb* lpPCB, ::pbuf* lpBuffer, ::err_t errorCode)
		{
			auto lpSocket = reinterpret_cast<Socket*>(
				lpParam
			);

			return lpSocket->LWIP_TCP_OnReadComplete(
				lpPCB,
				lpBuffer,
				errorCode
			);
		}
		       ::err_t LWIP_TCP_OnReadComplete(::tcp_pcb* lpPCB, ::pbuf* lpBuffer, ::err_t errorCode)
		{
			if ((errorCode == ::ERR_VAL) || (lpPCB == nullptr))
			{
				LWIP_CompleteOperation(
					False,
					False,
					errorCode
				);

				return errorCode;
			}

			auto bufferSize = lpBuffer->tot_len;

			if (bufferSize > ioContext.RX.BufferSize)
			{

				bufferSize = ioContext.RX.BufferSize;
			}

			auto numberOfBytesReceived = ::pbuf_copy_partial(
				lpBuffer,
				reinterpret_cast<uint8*>(ioContext.RX.lpBuffer),
				bufferSize,
				0
			);

			::tcp_recved(
				socket.tcp,
				bufferSize
			);

			::pbuf_free(
				lpBuffer
			);

			ioContext.RX.NumberOfBytesReceived = numberOfBytesReceived;

			LWIP_CompleteOperation(
				True,
				False,
				::ERR_OK
			);

			return ::ERR_OK;
		}

		static ::err_t LWIP_TCP_OnWrite(void* lpParam, ::tcp_pcb* lpPCB, ::u16_t numberOfBytesSent)
		{
			auto lpSocket = reinterpret_cast<Socket*>(
				lpParam
			);

			return lpSocket->LWIP_TCP_OnWriteComplete(
				lpPCB,
				numberOfBytesSent
			);
		}
		       ::err_t LWIP_TCP_OnWriteComplete(::tcp_pcb* lpPCB, ::u16_t numberOfBytesSent)
		{
			if (lpPCB == nullptr)
			{
				LWIP_CompleteOperation(
					False,
					False,
					::ERR_VAL
				);

				return ::ERR_VAL;
			}

			ioContext.TX.NumberOfBytesSent = numberOfBytesSent;

			LWIP_CompleteOperation(
				True,
				False,
				::ERR_OK
			);

			return ::ERR_OK;
		}
#endif

		// @throw AL::Exception
		static Void GetLocalEndPoint(IPEndPoint& ep, Handle socket, SocketTypes socketType, AddressFamilies addressFamily)
		{
#if defined(AL_PLATFORM_PICO_W)
			switch (socketType)
			{
				case SocketTypes::Stream:
					if (auto lpPCB = reinterpret_cast<::tcp_pcb*>(socket))
					{
						ep.Host = IPAddress::FromNative(lpPCB->local_ip);
						ep.Port = lpPCB->local_port;
					}
					break;

				case SocketTypes::DataGram:
					if (auto lpPCB = reinterpret_cast<::udp_pcb*>(socket))
					{
						ep.Host = IPAddress::FromNative(lpPCB->local_ip);
						ep.Port = lpPCB->local_port;
					}
					break;

				default:
					throw NotImplementedException();
			}
#elif defined(AL_PLATFORM_LINUX) || defined(AL_PLATFORM_WINDOWS)
			switch (addressFamily)
			{
				case AddressFamilies::IPv4:
				{
					::sockaddr_in address;
					::socklen_t   addressSize = sizeof(sockaddr_in);

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
					::sockaddr_in6 address;
					::socklen_t    addressSize = sizeof(sockaddr_in6);

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
#endif
		}

		// @throw AL::Exception
		static Void GetRemoteEndPoint(IPEndPoint& ep, Handle socket, SocketTypes socketType, AddressFamilies addressFamily)
		{
#if defined(AL_PLATFORM_PICO_W)
			switch (socketType)
			{
				case SocketTypes::Stream:
					if (auto lpPCB = reinterpret_cast<::tcp_pcb*>(socket))
					{
						ep.Host = IPAddress::FromNative(lpPCB->remote_ip);
						ep.Port = lpPCB->remote_port;
					}
					break;

				case SocketTypes::DataGram:
					if (auto lpPCB = reinterpret_cast<::udp_pcb*>(socket))
					{
						ep.Host = IPAddress::FromNative(lpPCB->remote_ip);
						ep.Port = lpPCB->remote_port;
					}
					break;

				default:
					throw NotImplementedException();
			}
#elif defined(AL_PLATFORM_LINUX) || defined(AL_PLATFORM_WINDOWS)
			switch (addressFamily)
			{
				case AddressFamilies::IPv4:
				{
					switch (socketType)
					{
						case SocketTypes::Stream:
							break;

						case SocketTypes::DataGram:
							break;

						default:
							throw NotImplementedException();
					}
					::sockaddr_in address;
					::socklen_t   addressSize = sizeof(sockaddr_in);

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
					::sockaddr_in6 address;
					::socklen_t    addressSize = sizeof(sockaddr_in6);

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
#endif
		}
	};
}
