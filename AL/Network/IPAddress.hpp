#pragma once
#include "AL/Common.hpp"

#include "SocketException.hpp"

#include "AL/Collections/Array.hpp"

#if defined(AL_PLATFORM_PICO)
	#error Platform not supported
#elif defined(AL_PLATFORM_LINUX)
	#include <netdb.h>

	#include <arpa/inet.h>
#elif defined(AL_PLATFORM_WINDOWS)
	#include "WinSock.hpp"
#endif

namespace AL::Network
{
	enum class AddressFamilies
	{
		IPv4 = AF_INET,
		IPv6 = AF_INET6
	};

	class IPAddress
	{
		union Address
		{
			in_addr  v4;
			in6_addr v6;
		};

		Bool            isWinSockLoaded = False;

		Address         address;
		AddressFamilies addressFamily;

	public:
		static IPAddress Any()
		{
			return in_addr
			{
#if defined(AL_PLATFORM_LINUX)
				.s_addr = BitConverter::HostToNetwork(INADDR_ANY)
#elif defined(AL_PLATFORM_WINDOWS)
				.S_un =
				{
					.S_addr = BitConverter::HostToNetwork(INADDR_ANY)
				}
#endif
			};
		}
		static IPAddress Any6()
		{
			return in6addr_any;
		}

		static IPAddress Loopback()
		{
			return in_addr
			{
#if defined(AL_PLATFORM_LINUX)
				.s_addr = BitConverter::HostToNetwork(INADDR_LOOPBACK)
#elif defined(AL_PLATFORM_WINDOWS)
				.S_un =
				{
					.S_addr = BitConverter::HostToNetwork(INADDR_LOOPBACK)
				}
#endif
			};
		}
		static IPAddress Loopback6()
		{
			return in6addr_loopback;
		}

		// @throw AL::Exception
		static IPAddress FromString(const String& string)
		{
			int result;

			IPAddress address;

			if ((result = ::inet_pton(AF_INET, string.GetCString(), &address.address.v4)) <= 0)
			{
				if (result == -1)
				{

					throw SocketException(
						"inet_pton"
					);
				}

				if ((result = ::inet_pton(AF_INET6, string.GetCString(), &address.address.v6)) <= 0)
				{
					if (result == -1)
					{

						throw SocketException(
							"inet_pton"
						);
					}

					throw Exception(
						"'%s' is not a valid IPv4 or IPv6 address",
						string.GetCString()
					);
				}

				address.addressFamily = AddressFamilies::IPv6;
			}
			else
			{

				address.addressFamily = AddressFamilies::IPv4;
			}

			return address;
		}

		// @throw AL::Exception
		// @return False if not found
		static Bool Resolve(IPAddress& address, const String& hostname)
		{
			::addrinfo hint = { 0 };
			hint.ai_family = AF_UNSPEC;

			::addrinfo* lpResult;

			if (OS::ErrorCode errorCode = ::getaddrinfo(hostname.GetCString(), "", &hint, &lpResult))
			{
				switch (errorCode)
				{
					case EAI_FAIL:
					case EAI_AGAIN:
						return False;
				}

				throw SocketException(
					"getaddrinfo",
					errorCode
				);
			}

			switch (lpResult->ai_family)
			{
				case AF_INET:
					address.address.v4 = reinterpret_cast<const ::sockaddr_in*>(lpResult->ai_addr)->sin_addr;
					address.addressFamily = AddressFamilies::IPv4;
					break;

				case AF_INET6:
					address.address.v6 = reinterpret_cast<const ::sockaddr_in6*>(lpResult->ai_addr)->sin6_addr;
					address.addressFamily = AddressFamilies::IPv6;
					break;

				default:
					::freeaddrinfo(lpResult);
					throw OperationNotSupportedException();
			}

			::freeaddrinfo(
				lpResult
			);

			return True;
		}

		IPAddress()
			: isWinSockLoaded(
#if defined(AL_PLATFORM_LINUX)
				False
#elif defined(AL_PLATFORM_WINDOWS)
				WinSock::TryLoad()
#endif
			)
		{
		}

		IPAddress(uint32 address)
			: IPAddress(
#if defined(AL_PLATFORM_LINUX)
				in_addr
				{
					.s_addr = address
				}
#elif defined(AL_PLATFORM_WINDOWS)
				in_addr
				{
					.S_un =
					{
						.S_un_b =
						{
							.s_b1 = static_cast<UCHAR>((address & 0xFF000000) >> 24),
							.s_b2 = static_cast<UCHAR>((address & 0x00FF0000) >> 16),
							.s_b3 = static_cast<UCHAR>((address & 0x0000FF00) >> 8),
							.s_b4 = static_cast<UCHAR>((address & 0x000000FF) >> 0)
						}
					}
				}
#endif
			)
		{
		}

		IPAddress(in_addr&& address)
			: isWinSockLoaded(
#if defined(AL_PLATFORM_LINUX)
				False
#elif defined(AL_PLATFORM_WINDOWS)
				WinSock::TryLoad()
#endif
			),
			address{
				.v4 = Move(address)
			},
			addressFamily(
				AddressFamilies::IPv4
			)
		{
		}
		IPAddress(const in_addr& address)
			: IPAddress(
				in_addr(address)
			)
		{
		}

		IPAddress(in6_addr&& address)
			: isWinSockLoaded(
#if defined(AL_PLATFORM_LINUX)
				False
#elif defined(AL_PLATFORM_WINDOWS)
				WinSock::TryLoad()
#endif
			),
			address{
				.v6 = Move(address)
			},
			addressFamily(
				AddressFamilies::IPv6
			)
		{
		}
		IPAddress(const in6_addr& address)
			: IPAddress(
				in6_addr(address)
			)
		{
		}

		IPAddress(IPAddress&& address)
			: isWinSockLoaded(
				address.isWinSockLoaded
			),
			address(
				Move(address.address)
			),
			addressFamily(
				address.addressFamily
			)
		{
			address.isWinSockLoaded = False;
		}
		IPAddress(const IPAddress& address)
			: isWinSockLoaded(
#if defined(AL_PLATFORM_LINUX)
				False
#elif defined(AL_PLATFORM_WINDOWS)
				address.isWinSockLoaded && WinSock::TryLoad()
#endif
			),
			address(
				Move(address.address)
			),
			addressFamily(
				address.addressFamily
			)
		{
		}

		virtual ~IPAddress()
		{
#if defined(AL_PLATFORM_WINDOWS)
			if (isWinSockLoaded)
			{

				WinSock::Unload();
			}
#endif
		}

		Bool IsV4() const
		{
			if (GetFamily() != AddressFamilies::IPv4)
			{
				
				return False;
			}

			return True;
		}

		Bool IsV6() const
		{
			if (GetFamily() != AddressFamilies::IPv6)
			{
				
				return False;
			}

			return True;
		}

		Bool IsV4MappedToV6() const
		{
			if (GetFamily() != AddressFamilies::IPv6)
			{

				return False;
			}

			static constexpr uint8 V4_BUFFER_PREFIX[12] =
			{
				0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0xFF, 0xFF
			};

#if defined(AL_PLATFORM_LINUX)
			if (!memcmp(&address.v6.__in6_u.__u6_addr8[0], &V4_BUFFER_PREFIX[0], 12))
			{

				return False;
			}
#elif defined(AL_PLATFORM_WINDOWS)
			if (!memcmp(&address.v6.u.Byte[0], &V4_BUFFER_PREFIX[0], 12))
			{

				return False;
			}
#endif

			return True;
		}

		AddressFamilies GetFamily() const
		{
			return addressFamily;
		}

		auto& GetAddress() const
		{
			return address.v4;
		}

		auto& GetAddress6() const
		{
			return address.v6;
		}

		// @throw AL::Exception
		IPAddress Cast(AddressFamilies family) const
		{
			IPAddress address;

			switch (GetFamily())
			{
				case AddressFamilies::IPv4:
				{
					switch (family)
					{
						case AddressFamilies::IPv4:
							address.address.v4 = this->address.v4;
							address.addressFamily = AddressFamilies::IPv4;
							break;

						case AddressFamilies::IPv6:
						{
							memset(
#if defined(AL_PLATFORM_LINUX)
								&address.address.v6.__in6_u.__u6_addr8[0],
#elif defined(AL_PLATFORM_WINDOWS)
								&address.address.v6.u.Byte[0],
#endif
								0x00,
								10
							);

#if defined(AL_PLATFORM_LINUX)
							address.address.v6.__in6_u.__u6_addr8[10] = 0xFF;
							address.address.v6.__in6_u.__u6_addr8[11] = 0xFF;
#elif defined(AL_PLATFORM_WINDOWS)
							address.address.v6.u.Byte[10] = 0xFF;
							address.address.v6.u.Byte[11] = 0xFF;
#endif

							memcpy(
#if defined(AL_PLATFORM_LINUX)
								&address.address.v6.__in6_u.__u6_addr8[12],
#elif defined(AL_PLATFORM_WINDOWS)
								&address.address.v6.u.Byte[12],
#endif
								&this->address.v4.s_addr,
								4
							);

							address.addressFamily = AddressFamilies::IPv6;
						}
						break;

						default:
							throw OperationNotSupportedException();
					}
				}
				break;

				case AddressFamilies::IPv6:
				{
					switch (family)
					{
						case AddressFamilies::IPv4:
						{
							if (!IsV4MappedToV6())
							{

								throw OperationNotSupportedException();
							}

							address.address.v4.s_addr = *reinterpret_cast<const uint32*>(
#if defined(AL_PLATFORM_LINUX)
								&this->address.v6.__in6_u.__u6_addr8[12]
#elif defined(AL_PLATFORM_WINDOWS)
								&this->address.v6.u.Byte[12]
#endif
							);

							address.addressFamily = AddressFamilies::IPv4;
						}
						break;

						case AddressFamilies::IPv6:
							address.address.v6 = this->address.v6;
							address.addressFamily = AddressFamilies::IPv6;
							break;

						default:
							throw OperationNotSupportedException();
					}
				}
				break;

				default:
					throw OperationNotSupportedException();
			}

			return address;
		}

		// @throw AL::Exception
		String ToString() const
		{
			const void* lpAddress;
			int         addressFamily;
			socklen_t   addressStringLength;

			switch (GetFamily())
			{
				case AddressFamilies::IPv4:
					lpAddress = &address.v4;
					addressFamily = AF_INET;
					addressStringLength = INET_ADDRSTRLEN;
					break;

				case AddressFamilies::IPv6:
					lpAddress = &address.v6;
					addressFamily = AF_INET6;
					addressStringLength = INET6_ADDRSTRLEN;
					break;

				default:
					throw OperationNotSupportedException();
			}

			Collections::Array<String::Char> buffer(
				String::END,
				addressStringLength + 1
			);

			if (::inet_ntop(addressFamily, lpAddress, &buffer[0], addressStringLength) == nullptr)
			{

				throw SocketException(
					"inet_ntop"
				);
			}

			return String(
				&buffer[0],
				addressStringLength
			);
		}

		IPAddress& operator = (uint32 address)
		{
#if defined(AL_PLATFORM_WINDOWS)
			if (isWinSockLoaded)
			{

				WinSock::Unload();
			}
#endif

#if defined(AL_PLATFORM_LINUX)
			isWinSockLoaded = False;
#elif defined(AL_PLATFORM_WINDOWS)
			isWinSockLoaded = WinSock::TryLoad();
#endif

			this->address.v4.s_addr = address;
			addressFamily = AddressFamilies::IPv4;

			return *this;
		}

		IPAddress& operator = (in_addr&& address)
		{
#if defined(AL_PLATFORM_WINDOWS)
			if (isWinSockLoaded)
			{

				WinSock::Unload();
			}
#endif

#if defined(AL_PLATFORM_LINUX)
			isWinSockLoaded = False;
#elif defined(AL_PLATFORM_WINDOWS)
			isWinSockLoaded = WinSock::TryLoad();
#endif

			this->address.v4 = Move(address);
			addressFamily = AddressFamilies::IPv4;

			return *this;
		}
		IPAddress& operator = (const in_addr& address)
		{
#if defined(AL_PLATFORM_WINDOWS)
			if (isWinSockLoaded)
			{

				WinSock::Unload();
			}
#endif

#if defined(AL_PLATFORM_LINUX)
			isWinSockLoaded = False;
#elif defined(AL_PLATFORM_WINDOWS)
			isWinSockLoaded = WinSock::TryLoad();
#endif

			this->address.v4 = address;
			addressFamily = AddressFamilies::IPv4;

			return *this;
		}

		IPAddress& operator = (in6_addr&& address)
		{
#if defined(AL_PLATFORM_WINDOWS)
			if (isWinSockLoaded)
			{

				WinSock::Unload();
			}
#endif

#if defined(AL_PLATFORM_LINUX)
			isWinSockLoaded = False;
#elif defined(AL_PLATFORM_WINDOWS)
			isWinSockLoaded = WinSock::TryLoad();
#endif

			this->address.v6 = Move(address);
			addressFamily = AddressFamilies::IPv6;

			return *this;
		}
		IPAddress& operator = (const in6_addr& address)
		{
#if defined(AL_PLATFORM_WINDOWS)
			if (isWinSockLoaded)
			{

				WinSock::Unload();
			}
#endif

#if defined(AL_PLATFORM_LINUX)
			isWinSockLoaded = False;
#elif defined(AL_PLATFORM_WINDOWS)
			isWinSockLoaded = WinSock::TryLoad();
#endif

			this->address.v6 = address;
			addressFamily = AddressFamilies::IPv6;

			return *this;
		}

		IPAddress& operator = (IPAddress&& address)
		{
#if defined(AL_PLATFORM_WINDOWS)
			if (isWinSockLoaded)
			{

				WinSock::Unload();
			}
#endif

			isWinSockLoaded = address.isWinSockLoaded;
			address.isWinSockLoaded = False;

			this->address = Move(address.address);
			addressFamily = address.addressFamily;

			return *this;
		}
		IPAddress& operator = (const IPAddress& address)
		{
#if defined(AL_PLATFORM_WINDOWS)
			if (isWinSockLoaded)
			{

				WinSock::Unload();
			}
#endif

#if defined(AL_PLATFORM_LINUX)
			isWinSockLoaded = False;
#elif defined(AL_PLATFORM_WINDOWS)
			isWinSockLoaded = address.isWinSockLoaded && WinSock::TryLoad();
#endif

			this->address = address.address;
			addressFamily = address.addressFamily;

			return *this;
		}

		Bool operator == (const IPAddress& address) const
		{
			auto family = GetFamily();

			if (family != address.GetFamily())
			{

				return False;
			}

			switch (family)
			{
				case AddressFamilies::IPv4:
				{
					if (!AL::memcmp(&this->address.v4.s_addr, &address.address.v4, sizeof(in_addr)))
					{

						return False;
					}
				}
				break;

				case AddressFamilies::IPv6:
				{
					if (!AL::memcmp(&this->address.v6, &address.address.v6, sizeof(in6_addr)))
					{

						return False;
					}
				}
				break;

				default:
					throw OperationNotSupportedException();
			}

			return True;
		}
		Bool operator != (const IPAddress& address) const
		{
			if (operator==(address))
			{

				return False;
			}

			return True;
		}
	};
}
