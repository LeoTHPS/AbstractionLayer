#pragma once
#include "AL/Common.hpp"

#include "IPAddress.hpp"
#include "IPEndPoint.hpp"

#include "ErrorCode.hpp"
#include "SocketException.hpp"

#if defined(AL_PLATFORM_WINDOWS)
	#include "WinSock.hpp"
#endif

namespace AL::Network
{
	enum class SocketTypes : uint8
	{
		TCP, UDP
	};

	enum class SocketShutdownTypes : uint8
	{
		Read,
		Write,
		ReadWrite
	};

	class ISocket
	{
		ISocket(const ISocket&) = delete;

	public:
		ISocket()
		{
		}

		virtual ~ISocket()
		{
		}

		virtual Bool IsOpen() const = 0;

		virtual SocketTypes GetType() const = 0;

		// @throw AL::Exception
		virtual Void Open() = 0;

		virtual Void Close() = 0;

	protected: // Utilities
#if defined(AL_PLATFORM_LINUX) || defined(AL_PLATFORM_WINDOWS)
		struct NativeSocketAddress
		{
			socklen_t          Size;
			union
			{
				::sockaddr_in  V4;
				::sockaddr_in6 V6;
				::sockaddr     addr;
			}                  Address;
		};

		static NativeSocketAddress GetNativeSocketAddress(const IPEndPoint& ep)
		{
			NativeSocketAddress address;

			switch (ep.Host.GetFamily())
			{
				case AddressFamilies::IPv4:
				{
					address =
					{
						.Size    = sizeof(::sockaddr_in),
						.Address =
						{
							.V4  =
							{
								.sin_family = AF_INET,
								.sin_port   = BitConverter::HostToNetwork(ep.Port),
								.sin_addr   = ep.Host.GetAddress()
							}
						}
					};
				}
				break;

				case AddressFamilies::IPv6:
				{
					address =
					{
						.Size    = sizeof(::sockaddr_in6),
						.Address =
						{
							.V6  =
							{
								.sin6_family = AF_INET6,
								.sin6_port   = BitConverter::HostToNetwork(ep.Port),
								.sin6_addr   = ep.Host.GetAddress6()
							}
						}
					};
				}
				break;

				default:
					throw NotImplementedException();
			}

			return address;
		}

		template<typename T_SOCKET_HANDLE>
		static Void GetLocalEndPoint(IPEndPoint& ep, T_SOCKET_HANDLE hSocket, SocketTypes type, AddressFamilies addressFamily)
		{
			switch (addressFamily)
			{
				case AddressFamilies::IPv4:
				{
					::sockaddr_in address;
					::socklen_t   addressSize = sizeof(::sockaddr_in);

	#if defined(AL_PLATFORM_LINUX)
					if (::getsockname(hSocket, reinterpret_cast<sockaddr*>(&address), &addressSize) == -1)
					{

						throw SocketException(
							"getsockname"
						);
					}
	#elif defined(AL_PLATFORM_WINDOWS)
					if (::getsockname(hSocket, reinterpret_cast<sockaddr*>(&address), &addressSize) == SOCKET_ERROR)
					{

						throw SocketException(
							"getsockname"
						);
					}
	#endif

					ep =
					{
						.Host = address.sin_addr,
						.Port = BitConverter::NetworkToHost(address.sin_port)
					};
				}
				break;

				case AddressFamilies::IPv6:
				{
					::sockaddr_in6 address;
					::socklen_t    addressSize = sizeof(sockaddr_in6);

	#if defined(AL_PLATFORM_LINUX)
					if (::getsockname(hSocket, reinterpret_cast<sockaddr*>(&address), &addressSize) == -1)
					{

						throw SocketException(
							"getsockname"
						);
					}
	#elif defined(AL_PLATFORM_WINDOWS)
					if (::getsockname(hSocket, reinterpret_cast<sockaddr*>(&address), &addressSize) == SOCKET_ERROR)
					{

						throw SocketException(
							"getsockname"
						);
					}
	#endif

					ep =
					{
						.Host = address.sin6_addr,
						.Port = BitConverter::NetworkToHost(address.sin6_port)
					};
				}
				break;

				default:
					throw NotImplementedException();
			}
		}

		template<typename T_SOCKET_HANDLE>
		static Void GetRemoteEndPoint(IPEndPoint& ep, T_SOCKET_HANDLE hSocket, SocketTypes type, AddressFamilies addressFamily)
		{
			switch (addressFamily)
			{
				case AddressFamilies::IPv4:
				{
					::sockaddr_in address;
					::socklen_t   addressSize = sizeof(sockaddr_in);

	#if defined(AL_PLATFORM_LINUX)
					if (::getpeername(hSocket, reinterpret_cast<sockaddr*>(&address), &addressSize) == -1)
					{

						throw SocketException(
							"getpeername"
						);
					}
	#elif defined(AL_PLATFORM_WINDOWS)
					if (::getpeername(hSocket, reinterpret_cast<sockaddr*>(&address), &addressSize) == SOCKET_ERROR)
					{

						throw SocketException(
							"getpeername"
						);
					}
	#endif

					ep =
					{
						.Host = address.sin_addr,
						.Port = BitConverter::NetworkToHost(address.sin_port)
					};
				}
				break;

				case AddressFamilies::IPv6:
				{
					::sockaddr_in6 address;
					::socklen_t    addressSize = sizeof(sockaddr_in6);

	#if defined(AL_PLATFORM_LINUX)
					if (::getpeername(hSocket, reinterpret_cast<sockaddr*>(&address), &addressSize) == -1)
					{

						throw SocketException(
							"getpeername"
						);
					}
	#elif defined(AL_PLATFORM_WINDOWS)
					if (::getpeername(hSocket, reinterpret_cast<sockaddr*>(&address), &addressSize) == SOCKET_ERROR)
					{

						throw SocketException(
							"getpeername"
						);
					}
	#endif

					ep =
					{
						.Host = address.sin6_addr,
						.Port = BitConverter::NetworkToHost(address.sin6_port)
					};
				}
				break;

				default:
					throw NotImplementedException();
			}
		}
#endif
	};
}
