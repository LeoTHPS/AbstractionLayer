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

#include "AL/Tasks/TaskThreadPool.hpp"

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

	typedef Function<void(void* lpBuffer, size_t bytesTransferred, Exceptions::Exception* lpException)> SocketAsyncReadCallback;
	typedef Function<void(const void* lpBuffer, size_t bytesTransferred, Exceptions::Exception* lpException)> SocketAsyncWriteCallback;

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
		bool isBound = false;
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
			socket.isBound = false;
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

		bool IsBound() const
		{
			return isBound;
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
		void Open()
		{
			AL_ASSERT(!IsOpen(), "Socket already open");

#if defined(AL_PLATFORM_LINUX)
			if ((hSocket = socket(static_cast<int>(GetAddressFamily()), static_cast<int>(GetType()), static_cast<int>(GetProtocol()))) == INVALID_SOCKET)
			{

				throw Exceptions::SocketException(
					"socket"
				);
			}
#elif defined(AL_PLATFORM_WINDOWS)
			if ((hSocket = WSASocketW(static_cast<int>(GetAddressFamily()), static_cast<int>(GetType()), static_cast<int>(GetProtocol()), nullptr, 0, 0)) == INVALID_SOCKET)
			{

				throw Exceptions::SocketException(
					"WSASocketW"
				);
			}
#endif

			isOpen = true;

			localEndPoint = IPEndPoint();
			remoteEndPoint = IPEndPoint();
		}

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
				isBound = false;
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
		void Bind(const IPEndPoint& localEP)
		{
			AL_ASSERT(IsOpen(), "Socket not open");
			AL_ASSERT(!IsBound(), "Socket already bound");

			AL_ASSERT(GetAddressFamily() == localEP.Address.GetFamily(), "Invalid AddressFamily");

			auto addressFamily = GetAddressFamily();

			if (addressFamily == AddressFamilies::IPv6)
			{
				int v6_only = 0;

				if (setsockopt(GetHandle(), IPPROTO_IPV6, IPV6_V6ONLY, reinterpret_cast<const char*>(&v6_only), sizeof(v6_only)) == SOCKET_ERROR)
				{

					throw Exceptions::SocketException(
						"setsockopt"
					);
				}
			}

			union
			{
				typename _Socket_SocketAddress<AddressFamilies::IPv4>::Type address;
				typename _Socket_SocketAddress<AddressFamilies::IPv6>::Type address6 = { 0 };
			};

			int addressSize = 0;

			switch (addressFamily)
			{
				case AddressFamilies::IPv4:
					addressSize = sizeof(address);
					_Socket_SocketAddress<AddressFamilies::IPv4>::SetPort(address, localEP.Port);
					_Socket_SocketAddress<AddressFamilies::IPv4>::SetFamily(address, AddressFamilies::IPv4);
					_Socket_SocketAddress<AddressFamilies::IPv4>::SetAddress(address, localEP.Address.GetAddress());
					break;

				case AddressFamilies::IPv6:
					addressSize = sizeof(address6);
					_Socket_SocketAddress<AddressFamilies::IPv6>::SetPort(address6, localEP.Port);
					_Socket_SocketAddress<AddressFamilies::IPv6>::SetFamily(address6, AddressFamilies::IPv6);
					_Socket_SocketAddress<AddressFamilies::IPv6>::SetAddress(address6, localEP.Address.GetAddress6());
					break;
			}

			if (bind(GetHandle(), reinterpret_cast<const sockaddr*>(&address), addressSize) == SOCKET_ERROR)
			{

				throw Exceptions::SocketException(
					"bind"
				);
			}

			switch (addressFamily)
			{
				case AddressFamilies::IPv4:
					localEndPoint = IPEndPoint(
						_Socket_SocketAddress<AddressFamilies::IPv4>::GetAddress(
							address
						),
						_Socket_SocketAddress<AddressFamilies::IPv4>::GetPort(
							address
						)
					);
					break;

				case AddressFamilies::IPv6:
					localEndPoint = IPEndPoint(
						_Socket_SocketAddress<AddressFamilies::IPv6>::GetAddress(
							address6
						),
						_Socket_SocketAddress<AddressFamilies::IPv6>::GetPort(
							address6
						)
					);
					break;
			}

			isBound = true;
		}

		// @throw AL::Exceptions::Exception
		void Listen(uint32 backlog = BACKLOG_MAX)
		{
			AL_ASSERT(IsOpen(), "Socket not open");
			AL_ASSERT(IsBound(), "Socket not bound");
			AL_ASSERT(!IsListening(), "Socket already listening");

			if (listen(GetHandle(), backlog) == SOCKET_ERROR)
			{

				throw Exceptions::SocketException(
					"listen"
				);
			}

			isListening = true;
		}

		// @throw AL::Exceptions::Exception
		bool Accept(Socket& socket) const
		{
			AL_ASSERT(IsOpen(), "Socket not open");
			AL_ASSERT(IsListening(), "Socket not listening");

			AL_ASSERT(GetType() == socket.GetType(), "Invalid SocketType");
			AL_ASSERT(GetProtocol() == socket.GetProtocol(), "Invalid SocketProtocol");
			AL_ASSERT(GetAddressFamily() == socket.GetAddressFamily(), "Invalid AddressFamily");

#if defined(AL_PLATFORM_LINUX)
			if ((socket.hSocket = accept(GetHandle(), nullptr, nullptr)) == INVALID_SOCKET)
			{
				auto lastError = GetLastError();

				if ((lastError != EAGAIN) && (lastError != EWOULDBLOCK))
				{

					throw Exceptions::SocketException(
						"accept",
						lastError
					);
				}

				return false;
			}
#elif defined(AL_PLATFORM_WINDOWS)
			if ((socket.hSocket = WSAAccept(GetHandle(), nullptr, nullptr, nullptr, 0)) == INVALID_SOCKET)
			{
				auto lastError = GetLastError();

				if (lastError != WSAEWOULDBLOCK)
				{

					throw Exceptions::SocketException(
						"WSAAccept",
						lastError
					);
				}

				return false;
			}
#endif

			try
			{
				switch (GetAddressFamily())
				{
					case AddressFamilies::IPv4:
						GetLocalEndPoint<AddressFamilies::IPv4>(socket.GetHandle(), socket.localEndPoint);
						GetRemoteEndPoint<AddressFamilies::IPv4>(socket.GetHandle(), socket.remoteEndPoint);
						break;

					case AddressFamilies::IPv6:
						GetLocalEndPoint<AddressFamilies::IPv6>(socket.GetHandle(), socket.localEndPoint);
						GetRemoteEndPoint<AddressFamilies::IPv6>(socket.GetHandle(), socket.remoteEndPoint);
						break;
				}
			}
			catch (Exceptions::Exception& exception)
			{
				socket.localEndPoint = IPEndPoint();
				socket.remoteEndPoint = IPEndPoint();

#if defined(AL_PLATFORM_LINUX)
				close(socket.GetHandle());
#elif defined(AL_PLATFORM_WINDOWS)
				closesocket(socket.GetHandle());
#endif

				socket.hSocket = INVALID_SOCKET;

				throw Exceptions::Exception(
					Move(exception),
					"Error getting end points"
				);
			}

#if defined(AL_PLATFORM_LINUX)
			try
			{
				socket.SetBlocking(
					IsBlocking()
				);
			}
			catch (Exceptions::Exception& exception)
			{
				socket.Close();

				throw Exceptions::Exception(
					Move(exception),
					"Error setting client blocking state"
				);
			}
#elif defined(AL_PLATFORM_WINDOWS)
			socket.isBlocking = IsBlocking();
#endif

			return true;
		}

		// @throw AL::Exceptions::Exception
		// @return false on timeout
		bool Connect(const IPEndPoint& remoteEP)
		{
			AL_ASSERT(IsOpen(), "Socket not open");

			AL_ASSERT(GetAddressFamily() == remoteEP.Address.GetFamily(), "Invalid AddressFamily");

			auto addressFamily = GetAddressFamily();

			union
			{
				typename _Socket_SocketAddress<AddressFamilies::IPv4>::Type address;
				typename _Socket_SocketAddress<AddressFamilies::IPv6>::Type address6 = { 0 };
			};

			int addressSize = 0;

			switch (addressFamily)
			{
				case AddressFamilies::IPv4:
					addressSize = sizeof(address);
					_Socket_SocketAddress<AddressFamilies::IPv4>::SetPort(address, remoteEP.Port);
					_Socket_SocketAddress<AddressFamilies::IPv4>::SetFamily(address, AddressFamilies::IPv4);
					_Socket_SocketAddress<AddressFamilies::IPv4>::SetAddress(address, remoteEP.Address.GetAddress());
					break;

				case AddressFamilies::IPv6:
					addressSize = sizeof(address6);
					_Socket_SocketAddress<AddressFamilies::IPv6>::SetPort(address6, remoteEP.Port);
					_Socket_SocketAddress<AddressFamilies::IPv6>::SetFamily(address6, AddressFamilies::IPv6);
					_Socket_SocketAddress<AddressFamilies::IPv6>::SetAddress(address6, remoteEP.Address.GetAddress6());
					break;
			}

#if defined(AL_PLATFORM_LINUX)
			if (connect(GetHandle(), reinterpret_cast<const sockaddr*>(&address), addressSize) == SOCKET_ERROR)
			{
				auto lastError = GetLastError();

				switch (lastError)
				{
					case ETIMEDOUT:
					case ENETUNREACH:
					case EHOSTDOWN:
					case EHOSTUNREACH:
						return false;
				}

				throw Exceptions::SocketException(
					"connect",
					lastError
				);
			}
#elif defined(AL_PLATFORM_WINDOWS)
			if (connect(GetHandle(), reinterpret_cast<const sockaddr*>(&address), addressSize) == SOCKET_ERROR)
			{
				auto lastError = GetLastError();

				switch (lastError)
				{
					case WSAETIMEDOUT:
					case WSAECONNREFUSED:
						return false;
				}

				throw Exceptions::SocketException(
					"connect",
					lastError
				);
			}
#endif

			try
			{
				switch (GetAddressFamily())
				{
					case AddressFamilies::IPv4:
						GetLocalEndPoint<AddressFamilies::IPv4>(GetHandle(), localEndPoint);
						GetRemoteEndPoint<AddressFamilies::IPv4>(GetHandle(), remoteEndPoint);
						break;

					case AddressFamilies::IPv6:
						GetLocalEndPoint<AddressFamilies::IPv6>(GetHandle(), localEndPoint);
						GetRemoteEndPoint<AddressFamilies::IPv6>(GetHandle(), remoteEndPoint);
						break;
				}
			}
			catch (Exceptions::Exception& exception)
			{
				Close();

				throw Exceptions::Exception(
					Move(exception),
					"Error getting end points"
				);
			}

			isConnected = true;

			return true;
		}

		// @throw AL::Exceptions::Exception
		// @return -1 if would block
		// @return 0 on connection closed
		// @return number of bytes received
		size_t Read(void* lpBuffer, size_t size)
		{
			AL_ASSERT(IsOpen(), "Socket not open");
			AL_ASSERT(IsConnected(), "Socket not connected");

			int bytesRead = recv(
				GetHandle(),
				static_cast<char*>(lpBuffer),
				static_cast<int>(size),
				0
			);

			if (bytesRead == SOCKET_ERROR)
			{
#if defined(AL_PLATFORM_LINUX)
				auto errorCode = GetLastError();

				if ((errorCode != EAGAIN) && (errorCode != EWOULDBLOCK))
				{
					switch (errorCode)
					{
						case ECONNRESET:
						case EHOSTDOWN:
						case EHOSTUNREACH:
							Close();
							return 0;
					}

					throw Exceptions::SocketException(
						"recv",
						errorCode
					);
				}

				bytesRead = -1;
#elif defined(AL_PLATFORM_WINDOWS)
				switch (auto errorCode = GetLastError())
				{
					case WSAEWOULDBLOCK:
						bytesRead = -1;
						break;

					case WSAENETDOWN:
					case WSAENETRESET:
					case WSAETIMEDOUT:
					case WSAECONNRESET:
					case WSAECONNABORTED:
						Close();
						break;

					default:
						throw Exceptions::SocketException(
							"recv",
							errorCode
						);
				}
#endif
			}

			else if (bytesRead == 0)
			{

				Close();
			}

			return static_cast<size_t>(
				bytesRead
			);
		}

		// @throw AL::Exceptions::Exception
		// @return -1 if would block
		// @return 0 on connection closed
		// @return number of bytes transmitted
		size_t Write(const void* lpBuffer, size_t size)
		{
			AL_ASSERT(IsOpen(), "Socket not open");
			AL_ASSERT(IsConnected(), "Socket not connected");

			int bytesSent = send(
				GetHandle(),
				static_cast<const char*>(lpBuffer),
				static_cast<int>(size),
				0
			);

			if (bytesSent == SOCKET_ERROR)
			{
#if defined(AL_PLATFORM_LINUX)
				auto errorCode = GetLastError();

				if ((errorCode != EAGAIN) && (errorCode != EWOULDBLOCK))
				{
					switch (errorCode)
					{
						case ECONNRESET:
						case EHOSTDOWN:
						case EHOSTUNREACH:
							Close();
							return 0;
					}

					throw Exceptions::SocketException(
						"send",
						errorCode
					);
				}

				bytesSent = -1;
#elif defined(AL_PLATFORM_WINDOWS)
				switch (auto errorCode = GetLastError())
				{
					case WSAEWOULDBLOCK:
						bytesSent = -1;
						break;

					case WSAENETDOWN:
					case WSAENETRESET:
					case WSAETIMEDOUT:
					case WSAECONNRESET:
					case WSAECONNABORTED:
					case WSAEHOSTUNREACH:
						Close();
						bytesSent = 0;
						break;

					default:
						throw Exceptions::SocketException(
							"send",
							errorCode
						);
				}
#endif

			}

			else if (bytesSent == 0)
			{

				Close();
			}

			return static_cast<size_t>(
				bytesSent
			);
		}

		// @throw AL::Exceptions::Exception
		// @return false on connection closed
		bool ReadAsync(void* lpBuffer, size_t size, SocketAsyncReadCallback&& callback, Tasks::TaskThreadPool& threadPool)
		{
			AL_ASSERT(IsOpen(), "Socket not open");
			AL_ASSERT(IsConnected(), "Socket not connected");

			Tasks::Task task(
				[this, lpBuffer, size, callback = Move(callback)]()
				{
					size_t bytesTransferred;

					try
					{
						bytesTransferred = Read(
							lpBuffer,
							size
						);

						callback(lpBuffer, bytesTransferred, nullptr);
					}
					catch (Exceptions::Exception& exception)
					{

						callback(lpBuffer, 0, &exception);
					}
				}
			);

			threadPool.Post(
				Move(task)
			);

			return true;
		}

		// @throw AL::Exceptions::Exception
		// @return false on connection closed
		bool WriteAsync(const void* lpBuffer, size_t size, SocketAsyncWriteCallback&& callback, Tasks::TaskThreadPool& threadPool)
		{
			AL_ASSERT(IsOpen(), "Socket not open");
			AL_ASSERT(IsConnected(), "Socket not connected");

			Tasks::Task task(
				[this, lpBuffer, size, callback = Move(callback)]()
				{
					size_t bytesTransferred;

					try
					{
						bytesTransferred = Write(
							lpBuffer,
							size
						);

						callback(lpBuffer, bytesTransferred, nullptr);
					}
					catch (Exceptions::Exception& exception)
					{

						callback(lpBuffer, 0, &exception);
					}
				}
			);

			threadPool.Post(
				Move(task)
			);

			return true;
		}

		auto& operator = (Socket&& socket)
		{
			Close();

			isOpen = socket.isOpen;
			socket.isOpen = false;

			isBound = socket.isBound;
			socket.isBound = false;

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
