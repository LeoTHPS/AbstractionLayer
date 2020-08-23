#pragma once
#include "AL/Common.hpp"

#if defined(AL_PLATFORM_LINUX)
	#include <fcntl.h>
	#include <unistd.h>

	#include <arpa/inet.h>

	#include <sys/types.h>
	#include <sys/socket.h>
#elif defined(AL_PLATFORM_WINDOWS)
	#include "WinSock.hpp"

	#include <mstcpip.h>
#endif

#include "IPEndPoint.hpp"

#include "AL/Exceptions/SocketException.hpp"

namespace AL::Sockets
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
		Read  = SHUT_RD,
		Write = SHUT_WR,
		Both  = SHUT_RDWR
#elif defined(AL_PLATFORM_WINDOWS)
		Read  = SD_RECEIVE,
		Write = SD_SEND,
		Both  = SD_BOTH
#endif
	};

#if defined(AL_PLATFORM_WINDOWS)
	class Socket;

	struct SocketAsyncContext;

	typedef Function<void(SocketAsyncContext&)> SocketAsyncCallback;

	struct SocketAsyncContext
	{
		WSAOVERLAPPED  O;

		Socket* const  lpSocket;

		WSABUF         Buffer;
		BitMask<DWORD> Flags = 0;

		SocketAsyncCallback	Callback;

		SocketAsyncContext(Socket* lpSocket, void* lpBuffer, size_t bufferSize, SocketAsyncCallback&& callback)
			: lpSocket(lpSocket),
			Callback(Move(callback))
		{
			ZeroMemory(&O, sizeof(O));

			Buffer.buf = reinterpret_cast<CHAR*>(lpBuffer);
			Buffer.len = static_cast<ULONG>(bufferSize);
		}

		auto GetFlags() const
		{
			return static_cast<uint32>(
				O.Offset
			);
		}

		auto GetErrorCode() const
		{
			return static_cast<OS::ErrorCode>(
				O.OffsetHigh
			);
		}

		auto GetBufferSize() const
		{
			return static_cast<size_t>(
				Buffer.len
			);
		}

		auto GetBytesTransferred() const
		{
			return static_cast<size_t>(
				O.InternalHigh
			);
		}

		void SetErrorCode(uint32 value)
		{
			O.OffsetHigh = static_cast<DWORD>(
				value
			);
		}
	};
#endif

	template<AddressFamilies>
	struct _Socket_SocketAddress;
	template<>
	struct _Socket_SocketAddress<AddressFamilies::IPv4>
	{
		typedef sockaddr_in Type;

		static constexpr uint16 GetPort(const Type& type)
		{
			return BitConverter::NetworkToHost(
				type.sin_port
			);
		}

		static constexpr AddressFamilies GetFamily(const Type& type)
		{
			return static_cast<AddressFamilies>(
				type.sin_family
			);
		}

		static constexpr in_addr GetAddress(Type& type)
		{
			return type.sin_addr;
		}

		static void SetPort(Type& type, uint16 port)
		{
			type.sin_port = BitConverter::HostToNetwork(
				port
			);
		}

		static void SetFamily(Type& type, AddressFamilies family)
		{
			type.sin_family = static_cast<decltype(type.sin_family)>(
				family
			);
		}

		static void SetAddress(Type& type, in_addr address)
		{
			type.sin_addr.s_addr = address.s_addr;
		}
	};
	template<>
	struct _Socket_SocketAddress<AddressFamilies::IPv6>
	{
		typedef sockaddr_in6 Type;

		static constexpr uint16 GetPort(const Type& type)
		{
			return BitConverter::NetworkToHost(
				type.sin6_port
			);
		}

		static constexpr AddressFamilies GetFamily(const Type& type)
		{
			return static_cast<AddressFamilies>(
				type.sin6_family
			);
		}

		static constexpr in6_addr& GetAddress(Type& type)
		{
			return type.sin6_addr;
		}

		static void SetPort(Type& type, uint16 port)
		{
			type.sin6_port = BitConverter::HostToNetwork(
				port
			);
		}

		static void SetFamily(Type& type, AddressFamilies family)
		{
			type.sin6_family = static_cast<decltype(type.sin6_family)>(
				family
			);
		}

		static void SetAddress(Type& type, const in6_addr& address)
		{
			AL::memcpy(
				&type.sin6_addr,
				&address,
				sizeof(Type)
			);
		}
	};

	constexpr uint32 BACKLOG_MAX = SOMAXCONN;

	class Socket
	{
#if defined(AL_PLATFORM_LINUX)
		typedef int HSOCKET;

		static constexpr int SOCKET_ERROR = -1;

		static constexpr HSOCKET INVALID_SOCKET = -1;
#elif defined(AL_PLATFORM_WINDOWS)
		typedef SOCKET HSOCKET;

		template<typename F>
		static bool WSALoadExtension(const Socket& s, GUID guid, F* lpFunction)
		{
			DWORD nBytesReturned = 0;

			if (::WSAIoctl(
				s.hSocket,
				SIO_GET_EXTENSION_FUNCTION_POINTER,
				&guid,
				sizeof(guid),
				lpFunction,
				sizeof(F),
				&nBytesReturned,
				nullptr,
				nullptr
			) != 0)
			{

				return false;
			}

			return nBytesReturned == sizeof(F);
		}

		static BOOL PASCAL FAR ConnectEx(__in const Socket& s, __in_bcount(namelen) const struct sockaddr FAR *name, __in int namelen, __in_opt PVOID lpSendBuffer, __in DWORD dwSendDataLength, __out LPDWORD lpdwBytesSent, __inout LPOVERLAPPED lpOverlapped)
		{
			LPFN_CONNECTEX function;

			if (WSALoadExtension(s, WSAID_CONNECTEX, &function))
			{
				return function(
					s.hSocket,
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

		static BOOL PASCAL FAR AcceptEx(__in const Socket& sListenSocket, __in const Socket& sAcceptSocket, __in PVOID lpOutputBuffer, __in DWORD dwReceiveDataLength, __in DWORD dwLocalAddressLength, __in DWORD dwRemoteAddressLength, __out LPDWORD lpdwBytesReceived, __inout LPOVERLAPPED lpOverlapped)
		{
			LPFN_ACCEPTEX function;

			if (WSALoadExtension(sListenSocket, WSAID_ACCEPTEX, &function))
			{
				return function(
					sListenSocket.hSocket,
					sAcceptSocket.hSocket,
					lpOutputBuffer,
					dwReceiveDataLength,
					dwLocalAddressLength,
					dwRemoteAddressLength,
					lpdwBytesReceived,
					lpOverlapped
				);
			}

			return FALSE;
		}

		static BOOL PASCAL FAR GetAcceptExSockaddrs(__in const Socket& s, __in PVOID lpOutputBuffer, __in DWORD dwReceiveDataLength, __in DWORD dwLocalAddressLength, __in DWORD dwRemoteAddressLength, __deref_out_bcount(*LocalSockaddrLength) struct sockaddr **LocalSockaddr, __out LPINT LocalSockaddrLength, __deref_out_bcount(*RemoteSockaddrLength) struct sockaddr **RemoteSockaddr, __out LPINT RemoteSockaddrLength)
		{
			LPFN_GETACCEPTEXSOCKADDRS function;

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

		static BOOL PASCAL FAR DisconnectEx(__in const Socket& s, __inout_opt LPOVERLAPPED lpOverlapped, __in DWORD  dwFlags, __in DWORD  dwReserved)
		{
			LPFN_DISCONNECTEX function;

			if (WSALoadExtension(s, WSAID_DISCONNECTEX, &function))
			{
				return function(
					s.hSocket,
					lpOverlapped,
					dwFlags,
					dwReserved
				);
			}

			return FALSE;
		}

		static BOOL PASCAL FAR TransmitFile(__in const Socket& s, __in HANDLE hFile, __in DWORD nNumberOfBytesToWrite, __in DWORD nNumberOfBytesPerSend, __inout_opt LPOVERLAPPED lpOverlapped, __in_opt LPTRANSMIT_FILE_BUFFERS lpTransmitBuffers, __in DWORD dwReserved)
		{
			LPFN_TRANSMITFILE function;

			if (WSALoadExtension(s, WSAID_TRANSMITFILE, &function))
			{
				return function(
					s.hSocket,
					hFile,
					nNumberOfBytesToWrite,
					nNumberOfBytesPerSend,
					lpOverlapped,
					lpTransmitBuffers,
					dwReserved
				);
			}

			return FALSE;
		}
#endif

		bool isOpen = false;
		bool isBlocking = true;
		bool isConnected = false;
		bool isListening = false;

		HSOCKET hSocket;
		SocketTypes type;
		SocketProtocols protocol;
		AddressFamilies addrFamily;

		IPEndPoint localEndPoint;
		IPEndPoint remoteEndPoint;

		Socket(const Socket&) = delete;

	public:
		Socket(Socket&& socket)
			: isOpen(
				socket.isOpen
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
			hSocket(
				socket.hSocket
			),
			type(
				socket.type
			),
			protocol(
				socket.protocol
			),
			addrFamily(
				socket.addrFamily
			),
			localEndPoint(
				Move(socket.localEndPoint)
			),
			remoteEndPoint(
				Move(socket.remoteEndPoint)
			)
		{
			socket.hSocket = INVALID_SOCKET;

			socket.isOpen = false;
			socket.isBlocking = true;
			socket.isConnected = false;
			socket.isListening = false;
		}

		Socket(SocketTypes type, SocketProtocols protocol, AddressFamilies family)
			: hSocket(
				INVALID_SOCKET
			),
			type(
				type
			),
			protocol(
				protocol
			),
			addrFamily(
				family
			)
		{
		}

		virtual ~Socket()
		{
			Close();
		}

		bool IsOpen() const
		{
			return isOpen;
		}

		bool IsBlocking() const
		{
			return isBlocking;
		}

		bool IsListening() const
		{
			return isListening;
		}

		bool IsConnected() const
		{
			return isConnected;
		}

		auto GetHandle() const
		{
			return hSocket;
		}

		auto GetType() const
		{
			return type;
		}

		auto GetProtocol() const
		{
			return protocol;
		}

		auto GetAddressFamily() const
		{
			return addrFamily;
		}

		auto& GetLocalEndPoint() const
		{
			return localEndPoint;
		}

		auto& GetRemoteEndPoint() const
		{
			return remoteEndPoint;
		}

		// @throw AL::Exceptions::Exception
		void SetBlocking(bool set = true)
		{
			if (set != IsBlocking())
			{
				if (IsOpen())
				{
#if defined(AL_PLATFORM_LINUX)
					int socketFlags = fcntl(
						GetHandle(),
						F_GETFL,
						0
					);

					if (fcntl(GetHandle(), F_SETFL, (!set ? (socketFlags | O_NONBLOCK) : (socketFlags & ~O_NONBLOCK))) == SOCKET_ERROR)
					{

						throw Exceptions::SocketException(
							"fcntl"
						);
					}
#elif defined(AL_PLATFORM_WINDOWS)
					u_long arg = set ? 0 : 1;

					if (ioctlsocket(GetHandle(), FIONBIO, &arg) == SOCKET_ERROR)
					{

						throw Exceptions::SocketException(
							"ioctlsocket"
						);
					}
#endif
				}

				isBlocking = set;
			}
		}

		// @throw AL::Exceptions::Exception
		void Open();

		void Close()
		{
			if (IsOpen())
			{
#if defined(AL_PLATFORM_LINUX)
				close(GetHandle());
#elif defined(AL_PLATFORM_WINDOWS)
				closesocket(GetHandle());
#endif

				isOpen = false;
				isBlocking = true;
				isConnected = false;
				isListening = false;

				hSocket = INVALID_SOCKET;
			}
		}

		// @throw AL::Exceptions::Exception
		void Shutdown(ShutdownTypes type)
		{
			AL_ASSERT(IsConnected(), "Socket not connected");

			if (shutdown(GetHandle(), static_cast<int>(type)) == SOCKET_ERROR)
			{

				throw Exceptions::SocketException(
					"shutdown"
				);
			}
		}

		// @throw AL::Exceptions::Exception
		void Bind(const IPEndPoint& localEP);

		// @throw AL::Exceptions::Exception
		void Listen(uint32 backlog = BACKLOG_MAX);

		// @throw AL::Exceptions::Exception
		bool Accept(Socket& socket) const;

		// @throw AL::Exceptions::Exception
		// @return false on timeout
		bool Connect(const IPEndPoint& remoteEP);

		// @throw AL::Exceptions::Exception
		// @return -1 if would block
		// @return 0 on connection closed
		// @return number of bytes received
		size_t Read(void* lpBuffer, size_t size);

		// @throw AL::Exceptions::Exception
		// @return -1 if would block
		// @return 0 on connection closed
		// @return number of bytes transmitted
		size_t Write(void* lpBuffer, size_t size);

#if defined(AL_PLATFORM_WINDOWS)
		// @throw AL::Exceptions::Exception
		// @return -1 if would block
		// @return 0 on connection closed
		// @return number of bytes enqueued
		size_t ReadAsync(void* lpBuffer, size_t size, SocketAsyncCallback&& callback);

		// @throw AL::Exceptions::Exception
		// @return -1 if would block
		// @return 0 on connection closed
		// @return number of bytes enqueued
		size_t WriteAsync(const void* lpBuffer, size_t size, SocketAsyncCallback&& callback);
#endif

		auto& operator = (Socket&& socket)
		{
			Close();

			isOpen = socket.isOpen;
			socket.isOpen = false;

			isBlocking = socket.isBlocking;
			socket.isBlocking = true;

			isConnected = socket.isConnected;
			socket.isConnected = false;

			isListening = socket.isListening;
			socket.isListening = false;

			hSocket = socket.hSocket;
			socket.hSocket = INVALID_SOCKET;

			type = socket.type;
			protocol = socket.protocol;
			addrFamily = socket.addrFamily;

			localEndPoint = Move(
				socket.localEndPoint
			);

			remoteEndPoint = Move(
				socket.remoteEndPoint
			);

			return *this;
		}

	private:
		// @throw AL::Exceptions::Exception
		void GetLocalEndPoint(HSOCKET& socket, IPEndPoint& ep)
		{
			switch (GetAddressFamily())
			{
				case AddressFamilies::IPv4:
					GetLocalEndPoint<AddressFamilies::IPv4>(socket, ep);
					break;

				case AddressFamilies::IPv6:
					GetLocalEndPoint<AddressFamilies::IPv6>(socket, ep);
					break;

				default:
					throw Exceptions::NotImplementedException();
			}
		}

		// @throw AL::Exceptions::Exception
		void GetRemoteEndPoint(HSOCKET& socket, IPEndPoint& ep)
		{
			switch (GetAddressFamily())
			{
				case AddressFamilies::IPv4:
					GetRemoteEndPoint<AddressFamilies::IPv4>(socket, ep);
					break;

				case AddressFamilies::IPv6:
					GetRemoteEndPoint<AddressFamilies::IPv6>(socket, ep);
					break;

				default:
					throw Exceptions::NotImplementedException();
			}
		}

		// @throw AL::Exceptions::Exception
		template<AddressFamilies ADDRESS_FAMILY>
		static void GetLocalEndPoint(HSOCKET socket, IPEndPoint& ep)
		{
			typename _Socket_SocketAddress<ADDRESS_FAMILY>::Type address;

			int addressSize = sizeof(address);

			if (getsockname(socket, reinterpret_cast<sockaddr*>(&address), &addressSize) == SOCKET_ERROR)
			{

				throw Exceptions::SocketException(
					"getsockname"
				);
			}

			ep = IPEndPoint(
				_Socket_SocketAddress<ADDRESS_FAMILY>::GetAddress(address),
				_Socket_SocketAddress<ADDRESS_FAMILY>::GetPort(address)
			);
		}

		// @throw AL::Exceptions::Exception
		template<AddressFamilies ADDRESS_FAMILY>
		static void GetRemoteEndPoint(HSOCKET socket, IPEndPoint& ep)
		{
			typename _Socket_SocketAddress<ADDRESS_FAMILY>::Type address;

			int addressSize = sizeof(address);

			if (getpeername(socket, reinterpret_cast<sockaddr*>(&address), &addressSize) == SOCKET_ERROR)
			{

				throw Exceptions::SocketException(
					"getpeername"
				);
			}

			ep = IPEndPoint(
				_Socket_SocketAddress<ADDRESS_FAMILY>::GetAddress(address),
				_Socket_SocketAddress<ADDRESS_FAMILY>::GetPort(address)
			);
		}
	};
}
