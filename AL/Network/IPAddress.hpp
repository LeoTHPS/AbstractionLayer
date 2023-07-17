#pragma once
#include "AL/Common.hpp"

#include "SocketException.hpp"

#include "AL/Collections/Array.hpp"

#if defined(AL_PLATFORM_PICO)
	#include "AL/Hardware/Pico/CYW43.hpp"

	#include <lwip/ip_addr.h>
#elif defined(AL_PLATFORM_LINUX)
	#include <netdb.h>

	#include <sys/un.h>

	#include <arpa/inet.h>
#elif defined(AL_PLATFORM_WINDOWS)
	#include "WinSock.hpp"
#else
	#error Platform not supported
#endif

namespace AL::Network
{
	enum class AddressFamilies : uint8
	{
#if defined(AL_PLATFORM_PICO)
		IPv4         = ::IPADDR_TYPE_V4,
		IPv6         = ::IPADDR_TYPE_V6,
		NotSpecified = ::IPADDR_TYPE_ANY
#elif defined(AL_PLATFORM_LINUX)
		IPv4         = AF_INET,
		IPv6         = AF_INET6,
		Unix         = AF_UNIX,
		NotSpecified = AF_UNSPEC
#elif defined(AL_PLATFORM_WINDOWS)
		IPv4         = AF_INET,
		IPv6         = AF_INET6,
		NotSpecified = AF_UNSPEC
#endif
	};

	class IPAddress
	{
		union Address
		{
#if defined(AL_PLATFORM_PICO)
	#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP_IPV4)
			::ip4_addr_t  v4;
	#endif
	#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP_IPV6)
			::ip6_addr_t  v6;
	#endif
#elif defined(AL_PLATFORM_LINUX)
			::in_addr     v4;
			::in6_addr    v6;
			::sockaddr_un Unix;
#elif defined(AL_PLATFORM_WINDOWS)
			::in_addr     v4;
			::in6_addr    v6;
#endif
		};

		Bool            isWinSockLoaded = False;

		Address         address;
		AddressFamilies addressFamily;

	public:
#if defined(AL_PLATFORM_PICO)
	#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP_IPV4) && defined(AL_DEPENDENCY_PICO_CYW43_LWIP_IPV6)
		typedef ::ip_addr_t  ip_addr;
		typedef ::ip4_addr_t in_addr;
		typedef ::ip6_addr_t in6_addr;
	#elif defined(AL_DEPENDENCY_PICO_CYW43_LWIP_IPV4)
		typedef ::ip_addr_t  ip_addr;
		typedef ::ip4_addr_t in_addr;
		typedef Void*        in6_addr;
	#elif defined(AL_DEPENDENCY_PICO_CYW43_LWIP_IPV6)
		typedef ::ip_addr_t  ip_addr;
		typedef Void*        in_addr;
		typedef ::ip6_addr_t in6_addr;
	#else
		typedef Void*        ip_addr;
		typedef Void*        in_addr;
		typedef Void*        in6_addr;
	#endif
#elif defined(AL_PLATFORM_LINUX) || defined(AL_PLATFORM_WINDOWS)
		typedef ::sockaddr   ip_addr;
		typedef ::in_addr    in_addr;
		typedef ::in6_addr   in6_addr;

		#if defined(AL_PLATFORM_LINUX)
			typedef ::sockaddr_un unix_addr;
		#endif
#endif

		static auto Any()
		{
#if defined(AL_PLATFORM_PICO)
	#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP_IPV4)
			return IPAddress(
				BitConverter::HostToNetwork(IPADDR_ANY)
			);
	#endif
#elif defined(AL_PLATFORM_LINUX)
			return IPAddress(
				in_addr
				{
					.s_addr = BitConverter::HostToNetwork(INADDR_ANY)
				}
			);
#elif defined(AL_PLATFORM_WINDOWS)
			return IPAddress(
				in_addr
				{
					.S_un =
					{
						.S_addr = BitConverter::HostToNetwork(INADDR_ANY)
					}
				}
			);
#endif
		}
		static auto Any6()
		{
#if defined(AL_PLATFORM_PICO)
	#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP_IPV6)
			in6_addr addr;
			ip6_addr_set_any(&addr);

			return IPAddress(
				addr
			);
	#endif
#elif defined(AL_PLATFORM_LINUX) || defined(AL_PLATFORM_WINDOWS)
			return IPAddress(
				::in6addr_any
			);
#endif
		}

		static auto Loopback()
		{
#if defined(AL_PLATFORM_PICO)
	#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP_IPV4)
			return IPAddress(
				BitConverter::HostToNetwork(IPADDR_LOOPBACK)
			);
	#endif
#elif defined(AL_PLATFORM_LINUX)
			return IPAddress(
				in_addr
				{
					.s_addr = BitConverter::HostToNetwork(INADDR_LOOPBACK)
				}
			);
#elif defined(AL_PLATFORM_WINDOWS)
			return IPAddress( 
				in_addr
				{
					.S_un =
					{
						.S_addr = BitConverter::HostToNetwork(INADDR_LOOPBACK)
					}
				}
			);
#endif
		}
		static auto Loopback6()
		{
#if defined(AL_PLATFORM_PICO)
	#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP_IPV6)
			in6_addr addr;
			ip6_addr_set_loopback(&addr);

			return IPAddress(
				addr
			);
	#endif
#elif defined(AL_PLATFORM_LINUX) || defined(AL_PLATFORM_WINDOWS)
			return IPAddress(
				::in6addr_loopback
			);
#endif
		}

		// @throw AL::Exception
		static IPAddress FromNative(const ip_addr& value)
		{
			IPAddress address;

#if defined(AL_PLATFORM_PICO)
	#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP_IPV4) && defined(AL_DEPENDENCY_PICO_CYW43_LWIP_IPV6)
			switch (value.type)
			{
				case IPADDR_TYPE_V4:
					address = value.u_addr.ip4;
					break;

				case IPADDR_TYPE_V6:
					address = value.u_addr.ip6;
					break;

				default:
					throw OperationNotSupportedException();
			}
	#elif defined(AL_DEPENDENCY_PICO_CYW43_LWIP_IPV4) || defined(AL_DEPENDENCY_PICO_CYW43_LWIP_IPV6)
			address = value;
	#endif
#elif defined(AL_PLATFORM_LINUX) || defined(AL_PLATFORM_WINDOWS)
			// TODO: implement
			throw NotImplementedException();
#endif

			return address;
		}

		// @throw AL::Exception
		static IPAddress FromString(const String& value)
		{
			IPAddress address;

#if defined(AL_PLATFORM_PICO)
			address = ::ipaddr_addr(
				value.GetCString()
			);
#elif defined(AL_PLATFORM_LINUX) || defined(AL_PLATFORM_WINDOWS)
			int result;

			if ((result = ::inet_pton(AF_INET, value.GetCString(), &address.address.v4)) <= 0)
			{
				if (result == -1)
				{

					throw SocketException(
						"inet_pton"
					);
				}

				if ((result = ::inet_pton(AF_INET6, value.GetCString(), &address.address.v6)) <= 0)
				{
					if (result == -1)
					{

						throw SocketException(
							"inet_pton"
						);
					}

	#if defined(AL_PLATFORM_LINUX)
					// TODO: try unix family
	#endif

					throw Exception(
						"'%s' is not a valid IPv4 or IPv6 address",
						value.GetCString()
					);
				}

				address.addressFamily = AddressFamilies::IPv6;
			}
			else
			{

				address.addressFamily = AddressFamilies::IPv4;
			}
#endif

			return address;
		}

		IPAddress()
			: isWinSockLoaded(
#if defined(AL_PLATFORM_PICO)
				False
#elif defined(AL_PLATFORM_LINUX)
				False
#elif defined(AL_PLATFORM_WINDOWS)
				WinSock::TryLoad()
#endif
			)
		{
		}

		IPAddress(uint32 address)
			: IPAddress(
#if defined(AL_PLATFORM_PICO)
	#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP_IPV4)
				in_addr
				{
					.addr = address
				}
	#elif defined(AL_DEPENDENCY_PICO_CYW43_LWIP_IPV6)
				in6_addr
				{
					.addr =
					{
						0x00000000,
						0x00000000,
						0x00000000,
						address
					}
				}
	#endif
#elif defined(AL_PLATFORM_LINUX)
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
							.s_b1 = static_cast<::UCHAR>((address & 0xFF000000) >> 24),
							.s_b2 = static_cast<::UCHAR>((address & 0x00FF0000) >> 16),
							.s_b3 = static_cast<::UCHAR>((address & 0x0000FF00) >> 8),
							.s_b4 = static_cast<::UCHAR>((address & 0x000000FF) >> 0)
						}
					}
				}
#endif
			)
		{
		}

		IPAddress(in_addr&& address)
			: isWinSockLoaded(
#if defined(AL_PLATFORM_PICO)
				False
#elif defined(AL_PLATFORM_LINUX)
				False
#elif defined(AL_PLATFORM_WINDOWS)
				WinSock::TryLoad()
#endif
			),
#if defined(AL_PLATFORM_PICO)
	#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP_IPV4)
			address{
				.v4 = Move(address)
			},
	#endif
#elif defined(AL_PLATFORM_LINUX) || defined(AL_PLATFORM_WINDOWS)
			address{
				.v4 = Move(address)
			},
#endif
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
#if defined(AL_PLATFORM_PICO)
				False
#elif defined(AL_PLATFORM_LINUX)
				False
#elif defined(AL_PLATFORM_WINDOWS)
				WinSock::TryLoad()
#endif
			),
#if defined(AL_PLATFORM_PICO)
	#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP_IPV6)
			address{
				.v6 = Move(address)
			},
	#endif
#elif defined(AL_PLATFORM_LINUX) || defined(AL_PLATFORM_WINDOWS)
			address{
				.v6 = Move(address)
			},
#endif
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

#if defined(AL_PLATFORM_LINUX)
		IPAddress(unix_addr&& address)
			: isWinSockLoaded(
				False
			),
			address{
				.Unix = Move(address)
			},
			addressFamily(
				AddressFamilies::Unix
			)
		{
		}
		IPAddress(const unix_addr& address)
			: IPAddress(
				unix_addr(address)
			)
		{
		}
#endif

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
#if defined(AL_PLATFORM_PICO)
				False
#elif defined(AL_PLATFORM_LINUX)
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

		Bool IsUnix() const
		{
#if defined(AL_PLATFORM_LINUX)
			if (GetFamily() == AddressFamilies::Unix)
			{

				return True;
			}
#endif

			return False;
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

#if defined(AL_PLATFORM_PICO)
	#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP_IPV6)
			if (!ip6_addr_isipv4mappedipv6(&address.v6))
			{

				return False;
			}
	#else
			throw DependencyMissingException(
				"LWIP_IPV6"
			);
	#endif
#elif defined(AL_PLATFORM_LINUX)
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

#if defined(AL_PLATFORM_LINUX)
		auto& GetUnix() const
		{
			return address.Unix;
		}
#endif

		auto& GetAddress() const
		{
#if defined(AL_PLATFORM_PICO) && defined(AL_DEPENDENCY_PICO_CYW43_LWIP_IPV4)
			return address.v4;
#elif defined(AL_PLATFORM_LINUX) || defined(AL_PLATFORM_WINDOWS)
			return address.v4;
#else
			static constexpr void* NULL_VALUE = nullptr;

			return NULL_VALUE;
#endif
		}

		auto& GetAddress6() const
		{
#if defined(AL_PLATFORM_PICO) && defined(AL_DEPENDENCY_PICO_CYW43_LWIP_IPV6)
			return address.v6;
#elif defined(AL_PLATFORM_LINUX) || defined(AL_PLATFORM_WINDOWS)
			return address.v6;
#else
			static constexpr void* NULL_VALUE = nullptr;

			return NULL_VALUE;
#endif
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
						{
#if defined(AL_PLATFORM_PICO)
	#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP_IPV4)
							address.address.v4    = this->address.v4;
							address.addressFamily = AddressFamilies::IPv4;
	#else
							throw DependencyMissingException(
								"LWIP_IPV4"
							);
	#endif
#elif defined(AL_PLATFOM_LINUX) || defined(AL_PLATFORM_WINDOWS)
							address.address.v4    = this->address.v4;
							address.addressFamily = AddressFamilies::IPv4;
#endif
						}
						break;

						case AddressFamilies::IPv6:
						{
#if defined(AL_PLATFORM_PICO)
	#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP_IPV4) && defined(AL_DEPENDENCY_PICO_CYW43_LWIP_IPV6)
							memset(
								&address.address.v6.addr[0],
								0x00,
								10
							);

							memcpy(
								&reinterpret_cast<uint8*>(&address.address.v6.addr[0])[12],
								&this->address.v4.addr,
								4
							);
	#else
							throw DependencyMissingException(
								"LWIP_IPV4 + LWIP_IPV6"
							);
	#endif
#elif defined(AL_PLATFORM_LINUX)
							memset(
								&address.address.v6.__in6_u.__u6_addr8[0],
								0x00,
								10
							);

							memcpy(
								&address.address.v6.__in6_u.__u6_addr8[12],
								&this->address.v4.s_addr,
								4
							);

							address.address.v6.__in6_u.__u6_addr8[10] = 0xFF;
							address.address.v6.__in6_u.__u6_addr8[11] = 0xFF;
							address.addressFamily = AddressFamilies::IPv6;
#elif defined(AL_PLATFORM_WINDOWS)
							memset(
								&address.address.v6.u.Byte[0],
								0x00,
								10
							);

							memcpy(
								&address.address.v6.u.Byte[12],
								&this->address.v4.s_addr,
								4
							);

							address.address.v6.u.Byte[10] = 0xFF;
							address.address.v6.u.Byte[11] = 0xFF;
							address.addressFamily = AddressFamilies::IPv6;
#endif
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

#if defined(AL_PLATFORM_PICO)
	#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP_IPV4) && defined(AL_DEPENDENCY_PICO_CYW43_LWIP_IPV6)
							address.address.v4.addr = *reinterpret_cast<const uint32*>(
								reinterpret_cast<const uint8*>(&this->address.v6.addr[0])[12]
							);

							address.addressFamily = AddressFamilies::IPv4;
	#else
							throw DependencyMissingException(
								"LWIP_IPV4 + LWIP_IPV6"
							);
	#endif
#elif defined(AL_PLATFORM_LINUX)
							address.address.v4.s_addr = *reinterpret_cast<const uint32*>(
								&this->address.v6.__in6_u.__u6_addr8[12]
							);

							address.addressFamily = AddressFamilies::IPv4;
#elif defined(AL_PLATFORM_WINDOWS)
							address.address.v4.s_addr = *reinterpret_cast<const uint32*>(
								&this->address.v6.u.Byte[12]
							);

							address.addressFamily = AddressFamilies::IPv4;
#endif
						}
						break;

						case AddressFamilies::IPv6:
						{
#if defined(AL_PLATFORM_PICO)
	#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP_IPV6)
							address.address.v6    = this->address.v6;
							address.addressFamily = AddressFamilies::IPv6;
	#else
							throw DependencyMissingException(
								"LWIP_IPV6"
							);
	#endif
#elif defined(AL_PLATFOM_LINUX) || defined(AL_PLATFORM_WINDOWS)
							address.address.v6    = this->address.v6;
							address.addressFamily = AddressFamilies::IPv6;
#endif
						}
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
#if defined(AL_PLATFORM_PICO)
			switch (GetFamily())
			{
				case AddressFamilies::IPv4:
	#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP_IPV4)
					return ::ip4addr_ntoa(&address.v4);
	#else
					throw DependencyMissingException(
						"LWIP_IPV4"
					);
	#endif

				case AddressFamilies::IPv6:
	#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP_IPV6)
					return ::ip6addr_ntoa(&address.v6);
	#else
					throw DependencyMissingException(
						"LWIP_IPV6"
					);
	#endif

				default:
					throw OperationNotSupportedException();
			}
#elif defined(AL_PLATFORM_LINUX) || defined(AL_PLATFORM_WINDOWS)
			const void* lpAddress;
			int         addressFamily;
			::socklen_t addressStringLength;

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

	#if defined(AL_PLATFORM_LINUX)
				case AddressFamilies::Unix:
					// TODO: implement
					throw NotImplementedException();
	#endif

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
#endif
		}

		auto ToNative() const
		{
#if defined(AL_PLATFORM_PICO)
	#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP_IPV4) && defined(AL_DEPENDENCY_PICO_CYW43_LWIP_IPV6)
			ip_addr address = { 0 };

			switch (GetFamily())
			{
				case AddressFamilies::IPv4:
					address.type = IPADDR_TYPE_V4;
					address.u_addr.ip4 = GetAddress();
					break;

				case AddressFamilies::IPv6:
					address.type = IPADDR_TYPE_V6;
					address.u_addr.ip6 = GetAddress6();
					break;

				default:
					throw OperationNotSupportedException();
			}

			return address;
	#elif defined(AL_DEPENDENCY_PICO_CYW43_LWIP_IPV4)
			return GetAddress();
	#elif defined(AL_DEPENDENCY_PICO_CYW43_LWIP_IPV6)
			return GetAddress6();
	#endif
#elif defined(AL_PLATFORM_LINUX) || defined(AL_PLATFORM_WINDOWS)
			// TODO: implement
#endif
		}

		IPAddress& operator = (uint32 address)
		{
#if defined(AL_PLATFORM_WINDOWS)
			if (isWinSockLoaded)
			{

				WinSock::Unload();
			}
#endif

#if defined(AL_PLATFORM_PICO)
			isWinSockLoaded = False;
#elif defined(AL_PLATFORM_LINUX)
			isWinSockLoaded = False;
#elif defined(AL_PLATFORM_WINDOWS)
			isWinSockLoaded = WinSock::TryLoad();
#endif

#if defined(AL_PLATFORM_PICO)
	#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP_IPV4)
			this->address.v4.addr = address;
	#endif
#elif defined(AL_PLATFORM_LINUX) || defined(AL_PLATFORM_WINDOWS)
			this->address.v4.s_addr = address;
#endif

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

#if defined(AL_PLATFORM_PICO)
			isWinSockLoaded = False;
#elif defined(AL_PLATFORM_LINUX)
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

#if defined(AL_PLATFORM_PICO)
			isWinSockLoaded = False;
#elif defined(AL_PLATFORM_LINUX)
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

#if defined(AL_PLATFORM_PICO)
			isWinSockLoaded = False;
#elif defined(AL_PLATFORM_LINUX)
			isWinSockLoaded = False;
#elif defined(AL_PLATFORM_WINDOWS)
			isWinSockLoaded = WinSock::TryLoad();
#endif

#if defined(AL_PLATFORM_PICO)
	#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP_IPV6)
			this->address.v6 = Move(address);
	#endif
#elif defined(AL_PLATFORM_LINUX) || defined(AL_PLATFORM_WINDOWS)
			this->address.v6 = Move(address);
#endif

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

#if defined(AL_PLATFORM_PICO)
			isWinSockLoaded = False;
#elif defined(AL_PLATFORM_LINUX)
			isWinSockLoaded = False;
#elif defined(AL_PLATFORM_WINDOWS)
			isWinSockLoaded = WinSock::TryLoad();
#endif

#if defined(AL_PLATFORM_PICO)
	#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP_IPV6)
			this->address.v6 = address;
	#endif
#elif defined(AL_PLATFORM_LINUX) || defined(AL_PLATFORM_WINDOWS)
			this->address.v6 = address;
#endif

			addressFamily = AddressFamilies::IPv6;

			return *this;
		}

#if defined(AL_PLATFORM_LINUX)
		IPAddress& operator = (unix_addr&& address)
		{
			// TODO: implement

			throw NotImplementedException();

			return *this;
		}
		IPAddress& operator = (const unix_addr& address)
		{
			// TODO: implement

			throw NotImplementedException();

			return *this;
		}
#endif

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

#if defined(AL_PLATFORM_PICO)
			isWinSockLoaded = False;
#elif defined(AL_PLATFORM_LINUX)
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
#if defined(AL_PLATFORM_PICO)
	#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP_IPV4)
					if (!AL::memcmp(&this->address.v4.addr, &address.address.v4.addr, sizeof(in_addr::addr)))
					{

						return False;
					}
	#endif
#elif defined(AL_PLATFORM_LINUX) || defined(AL_PLATFORM_WINDOWS)
					if (!AL::memcmp(&this->address.v4.s_addr, &address.address.v4.s_addr, sizeof(in_addr::s_addr)))
					{

						return False;
					}
#endif
				}
				break;

				case AddressFamilies::IPv6:
				{
#if defined(AL_PLATFORM_PICO)
	#if defined(AL_DEPENDENCY_PICO_CYW43_LWIP_IPV6)
					if (!AL::memcmp(this->address.v6.addr, address.address.v6.addr))
					{

						return False;
					}
	#endif
#elif defined(AL_PLATFORM_LINUX) || defined(AL_PLATFORM_WINDOWS)
					if (!AL::memcmp(this->address.v6.s6_addr, address.address.v6.s6_addr))
					{

						return False;
					}
#endif

				}
				break;

#if defined(AL_PLATFORM_LINUX)
				case AddressFamilies::Unix:
				{
					// TODO: implement

					throw NotImplementedException();
				}
				break;
#endif

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
