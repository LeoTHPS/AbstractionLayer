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
#endif

namespace AL::Network
{
	class UdpSocket
		: public ISocket
	{
		Bool            isOpen      = False;
		Bool            isBound     = False;
		Bool            isBlocking  = True;
		Bool            isConnected = False;

		SocketTypes     type;

		IPEndPoint      localEP;
		IPEndPoint      remoteEP;
		AddressFamilies addressFamily;

#if defined(AL_PLATFORM_PICO_W)
		LWIP::UdpSocket socket;
#elif defined(AL_PLATFORM_LINUX)
		int             socket;
#elif defined(AL_PLATFORM_WINDOWS)
		::SOCKET        socket;
#endif

	public:
#if defined(AL_PLATFORM_PICO_W)
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
			isConnected(
				udpSocket.isConnected
			),
			type(
				udpSocket.type
			),
			localEP(
				Move(udpSocket.localEP)
			),
			remoteEP(
				Move(udpSocket.remoteEP)
			),
			addressFamily(
				udpSocket.addressFamily
			),
			socket(
				Move(udpSocket.socket)
			)
		{
			udpSocket.isOpen      = False;
			udpSocket.isBound     = False;
			udpSocket.isBlocking  = True;
			udpSocket.isConnected = False;
		}

		explicit UdpSocket(AddressFamilies addressFamily)
			: type(
				SocketTypes::UDP
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

		virtual ~UdpSocket()
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

		virtual SocketTypes GetType() const override
		{
			return SocketTypes::UDP;
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
				"UdpSocket already open"
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
			}
		}

		// @throw AL::Exception
		virtual Void Bind(const IPEndPoint& ep)
		{
			AL_ASSERT(
				IsOpen(),
				"UdpSocket not open"
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
					"Error binding LWIP::UdpSocket"
				);
			}

			localEP = socket.GetLocalEndPoint();
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

			localEP = ep;
#else
			throw NotImplementedException();
#endif

			isBound = True;
		}

		// @throw AL::Exception
		virtual Void Connect(const IPEndPoint& ep)
		{
			AL_ASSERT(
				IsOpen(),
				"UdpSocket not open"
			);

			AL_ASSERT(
				!IsConnected(),
				"UdpSocket already connected"
			);

#if defined(AL_PLATFORM_PICO_W)
			try
			{
				socket.Connect(
					ep
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error connecting LWIP::UdpSocket"
				);
			}

			remoteEP = socket.GetRemoteEndPoint();
#elif defined(AL_PLATFORM_LINUX) || defined(AL_PLATFORM_WINDOWS)
			remoteEP = ep;
#else
			throw NotImplementedException();
#endif

			isConnected = True;
		}

		virtual Void Disconnect()
		{
			if (IsOpen() && IsConnected())
			{
#if defined(AL_PLATFORM_PICO_W)
				socket.Disconnect();
#elif defined(AL_PLATFORM_LINUX)
				// do nothing
#elif defined(AL_PLATFORM_WINDOWS)
				// do nothing
#endif

				isConnected = False;
			}
		}

		// @throw AL::Exception
		// @return AL::False on no route to host
		virtual Bool Send(const Void* lpBuffer, size_t size, size_t& numberOfBytesSent)
		{
			AL_ASSERT(
				IsOpen(),
				"UdpSocket not open"
			);

			AL_ASSERT(
				IsConnected(),
				"UdpSocket not connected"
			);

#if defined(AL_PLATFORM_PICO_W)
			// TODO: emulate blocking state

			try
			{
				if (!socket.Send(lpBuffer, size, numberOfBytesSent))
				{

					return False;
				}
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
			// TODO: implement
			throw NotImplementedException();
#else
			throw NotImplementedException();
#endif

			return True;
		}

		// @throw AL::Exception
		// @return AL::False on no route to host
		virtual Bool SendTo(const Void* lpBuffer, size_t size, size_t& numberOfBytesSent, const IPEndPoint& ep)
		{
			AL_ASSERT(
				IsOpen(),
				"UdpSocket not open"
			);

#if defined(AL_PLATFORM_PICO_W)
			// TODO: emulate blocking state

			try
			{
				if (!socket.SendTo(lpBuffer, size, numberOfBytesSent, ep))
				{

					return False;
				}
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
			// TODO: implement
			throw NotImplementedException();
#else
			throw NotImplementedException();
#endif

			return True;
		}

		// @throw AL::Exception
		// @return AL::False on timeout
		virtual Bool Receive(Void* lpBuffer, size_t size, size_t& numberOfBytesReceived, IPEndPoint& ep)
		{
			AL_ASSERT(
				IsOpen(),
				"UdpSocket not open"
			);

#if defined(AL_PLATFORM_PICO_W)
			// TODO: emulate blocking state

			try
			{
				numberOfBytesReceived = socket.Receive(
					lpBuffer,
					size,
					ep
				);
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
			// TODO: implement
			throw NotImplementedException();
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
				// TODO: implement
				throw NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
				// TODO: implement
				throw NotImplementedException();
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

			isConnected = udpSocket.isConnected;
			udpSocket.isConnected = False;

			type = udpSocket.type;

			localEP = Move(
				udpSocket.localEP
			);

			remoteEP = Move(
				udpSocket.remoteEP
			);

			addressFamily = udpSocket.addressFamily;

			socket = Move(
				udpSocket.socket
			);

			return *this;
		}
	};
}
