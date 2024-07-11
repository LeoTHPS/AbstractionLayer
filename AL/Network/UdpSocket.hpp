#pragma once
#include "AL/Common.hpp"

#include "ISocket.hpp"

#if defined(AL_PLATFORM_PICO)
	#include "LWIP.hpp"
#elif defined(AL_PLATFORM_LINUX)
	#include <fcntl.h>
	#include <unistd.h>

	#include <arpa/inet.h>

	#include <sys/types.h>
	#include <sys/socket.h>
#elif defined(AL_PLATFORM_WINDOWS)

#else
	#error Platform not supported
#endif

namespace AL::Network
{
	class UdpSocket
		: public ISocket
	{
		Bool            isOpen      = False;
		Bool            isBound     = False;
		Bool            isBlocking  = True;
#if defined(AL_PLATFORM_WINDOWS)
		Bool            isWinSockLoaded = False;
#endif

		SocketTypes     type;
		AddressFamilies addressFamily;

#if defined(AL_PLATFORM_PICO)
		LWIP::UdpSocket socket;
#elif defined(AL_PLATFORM_LINUX)
		int             socket;
#elif defined(AL_PLATFORM_WINDOWS)
		::SOCKET        socket;
#endif

	public:
#if defined(AL_PLATFORM_PICO)
		typedef typename LWIP::UdpSocket::Handle Handle;
#elif defined(AL_PLATFORM_LINUX)
		typedef int      Handle;
#elif defined(AL_PLATFORM_WINDOWS)
		typedef ::SOCKET Handle;
#else
		typedef Void*    Handle;
#endif

		UdpSocket(UdpSocket&& udpSocket)
			: isOpen(
				udpSocket.isOpen
			),
			isBound(
				udpSocket.isBound
			),
			isBlocking(
				udpSocket.isBlocking
			),
#if defined(AL_PLATFORM_WINDOWS)
			isWinSockLoaded(
				udpSocket.isWinSockLoaded
			),
#endif
			type(
				udpSocket.type
			),
			addressFamily(
				udpSocket.addressFamily
			),
			socket(
				Move(udpSocket.socket)
			)
		{
			udpSocket.isOpen          = False;
			udpSocket.isBound         = False;
			udpSocket.isBlocking      = True;
#if defined(AL_PLATFORM_WINDOWS)
			udpSocket.isWinSockLoaded = True;
#endif
		}

		explicit UdpSocket(AddressFamilies addressFamily)
			:
#if defined(AL_PLATFORM_WINDOWS)
			isWinSockLoaded(
				WinSock::TryLoad()
			),
#endif
			type(
				SocketTypes::UDP
			),
			addressFamily(
				addressFamily
			)
#if defined(AL_PLATFORM_PICO)
			,
			socket(
				addressFamily
			)
#endif
		{
		}

		virtual ~UdpSocket()
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

		virtual SocketTypes GetType() const override
		{
			return SocketTypes::UDP;
		}

		virtual Handle GetHandle() const
		{
#if defined(AL_PLATFORM_PICO)
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

		// @throw AL::Exception
		virtual Void Open() override
		{
			AL_ASSERT(
				!IsOpen(),
				"UdpSocket already open"
			);

#if defined(AL_PLATFORM_PICO)
			try
			{
				socket.Open();
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error opening LWIP::UdpSocket"
				);
			}
#elif defined(AL_PLATFORM_LINUX)
			if ((socket = ::socket(static_cast<typename Get_Enum_Or_Integer_Base<AddressFamilies>::Type>(GetAddressFamily()), SOCK_DGRAM, IPPROTO_UDP)) == -1)
			{

				throw SocketException(
					"socket"
				);
			}
#elif defined(AL_PLATFORM_WINDOWS)
			if ((socket = ::WSASocketW(static_cast<typename Get_Enum_Or_Integer_Base<AddressFamilies>::Type>(GetAddressFamily()), SOCK_DGRAM, IPPROTO_UDP, nullptr, 0, 0)) == INVALID_SOCKET)
			{

				throw SocketException(
					"WSASocketW"
				);
			}

			::BOOL broadcast = TRUE;

			if (::setsockopt(GetHandle(), SOL_SOCKET, SO_BROADCAST, reinterpret_cast<const char*>(&broadcast), sizeof(::BOOL)))
			{
				::closesocket(
					socket
				);

				throw SocketException(
					"setsockopt"
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
#if defined(AL_PLATFORM_PICO)
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

				isOpen  = False;
				isBound = False;
			}
		}

		// @throw AL::Exception
		virtual Void Bind(const IPEndPoint& ep)
		{
			AL_ASSERT(
				IsOpen(),
				"UdpSocket not open"
			);

#if defined(AL_PLATFORM_PICO)
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
					"Error binding LWIP::UdpSocket"
				);
			}
#elif defined(AL_PLATFORM_LINUX) || defined(AL_PLATFORM_WINDOWS)
			auto address = GetNativeSocketAddress(
				ep
			);

	#if defined(AL_PLATFORM_LINUX)
			if (::bind(GetHandle(), reinterpret_cast<::sockaddr*>(&address.Address.Storage), address.Size) == -1)
			{

				throw SocketException(
					"bind"
				);
			}
	#elif defined(AL_PLATFORM_WINDOWS)
			if (::bind(GetHandle(), reinterpret_cast<::sockaddr*>(&address.Address.Storage), address.Size) == SOCKET_ERROR)
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
		// @return number of bytes sent
		virtual size_t Send(const Void* lpBuffer, size_t size, const IPEndPoint& ep)
		{
			AL_ASSERT(
				IsOpen(),
				"UdpSocket not open"
			);

			size_t numberOfBytesSent = 0;

#if defined(AL_PLATFORM_PICO)
			try
			{
				do
				{
					if (!socket.SendTo(lpBuffer, size, numberOfBytesSent, ep))
					{

						throw Exception(
							"No route to host"
						);
					}
				} while (IsBlocking() && (numberOfBytesSent == 0));
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error sending LWIP::UdpSocket"
				);
			}
#elif defined(AL_PLATFORM_LINUX)
			// TODO: implement
			throw NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			auto addr = GetNativeSocketAddress(
				ep
			);

			int _numberOfBytesSent;

			if ((_numberOfBytesSent = ::sendto(GetHandle(), reinterpret_cast<const char*>(lpBuffer), static_cast<int>(size & Integer<int>::SignedCastMask), 0, reinterpret_cast<::sockaddr*>(&addr.Address.Storage), addr.Size)) == SOCKET_ERROR)
			{
				ErrorCode errorCode;

				if ((errorCode = GetLastError()) == WSAEWOULDBLOCK)
				{

					return 0;
				}

				Close();

				throw SocketException(
					"sendto",
					errorCode
				);
			}

			numberOfBytesSent = _numberOfBytesSent;
#else
			throw NotImplementedException();
#endif

			return numberOfBytesSent;
		}

		// @throw AL::Exception
		// @return number of bytes received
		virtual size_t Receive(Void* lpBuffer, size_t size, IPEndPoint& ep)
		{
			AL_ASSERT(
				IsOpen(),
				"UdpSocket not open"
			);

			size_t numberOfBytesReceived;

#if defined(AL_PLATFORM_PICO)
			try
			{
				while (((numberOfBytesReceived = socket.Receive(lpBuffer, size, ep)) == 0) && IsBlocking())
				{
				}
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error receiving LWIP::UdpSocket"
				);
			}
#elif defined(AL_PLATFORM_LINUX)
			// TODO: implement
			throw NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			NativeSocketAddress addr
			{
				.Size = sizeof(::sockaddr_storage)
			};

			int _numberOfBytesReceived;

			if ((_numberOfBytesReceived = ::recvfrom(GetHandle(), reinterpret_cast<char*>(lpBuffer), static_cast<int>(size & Integer<int>::SignedCastMask), 0, reinterpret_cast<::sockaddr*>(&addr.Address.Storage), &addr.Size)) == SOCKET_ERROR)
			{
				ErrorCode errorCode;

				if ((errorCode = GetLastError()) == WSAEWOULDBLOCK)
				{

					return 0;
				}

				Close();

				throw SocketException(
					"recvfrom",
					errorCode
				);
			}

			switch (addr.Address.Storage.ss_family)
			{
				case AF_INET:
					ep.Host = Move(addr.Address.V4.sin_addr);
					ep.Port = BitConverter::NetworkToHost(addr.Address.V4.sin_port);
					break;

				case AF_INET6:
					ep.Host = Move(addr.Address.V6.sin6_addr);
					ep.Port = BitConverter::NetworkToHost(addr.Address.V6.sin6_port);
					break;

				default:
					throw OperationNotSupportedException();
			}

			numberOfBytesReceived = _numberOfBytesReceived;
#else
			throw NotImplementedException();
#endif

			return numberOfBytesReceived;
		}

		// @throw AL::Exception
		virtual Void SetBlocking(Bool value)
		{
			if (IsOpen())
			{
#if defined(AL_PLATFORM_PICO)
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

		UdpSocket& operator = (UdpSocket&& udpSocket)
		{
			Close();

			isOpen = udpSocket.isOpen;
			udpSocket.isOpen = False;

			isBlocking = udpSocket.isBlocking;
			udpSocket.isBlocking = True;

			isBound = udpSocket.isBound;
			udpSocket.isBound = False;

#if defined(AL_PLATFORM_WINDOWS)
			isWinSockLoaded = udpSocket.isWinSockLoaded;
			udpSocket.isWinSockLoaded = False;
#endif

			type          = udpSocket.type;
			addressFamily = udpSocket.addressFamily;

			socket = Move(
				udpSocket.socket
			);

			return *this;
		}
	};
}
