#pragma once
#include "AL/Common.hpp"

#include "AL/Exceptions/SocketException.hpp"

#if defined(AL_PLATFORM_LINUX)
	#include <netdb.h>

	#include <arpa/inet.h>
#elif defined(AL_PLATFORM_WINDOWS)
	#include "WinSock.hpp"
#endif

namespace AL::Sockets
{
	enum class AddressFamilies
	{
		IPv4 = AF_INET,
		IPv6 = AF_INET6
	};

	template<AddressFamilies>
	struct _IPAddress_Helper;
	template<>
	struct _IPAddress_Helper<AddressFamilies::IPv4>
	{
		typedef in_addr Type;

		static constexpr int Family = AF_INET;

		static constexpr size_t Size = sizeof(Type);

		static constexpr size_t StringLength = INET_ADDRSTRLEN;

		static Type GetAddress(sockaddr* lpSockAddress)
		{
			return reinterpret_cast<sockaddr_in*>(
				lpSockAddress
			)->sin_addr;
		}

		static bool Compare(const Type& address, const Type& address2)
		{
			return address.s_addr == address2.s_addr;
		}
	};
	template<>
	struct _IPAddress_Helper<AddressFamilies::IPv6>
	{
		typedef in6_addr Type;

		static constexpr int Family = AF_INET6;

		static constexpr size_t Size = sizeof(Type);

		static constexpr size_t StringLength = INET6_ADDRSTRLEN;

		static Type GetAddress(sockaddr* lpSockAddress)
		{
			return reinterpret_cast<sockaddr_in6*>(
				lpSockAddress
			)->sin6_addr;
		}

		static bool Compare(const Type& address, const Type& address2)
		{
#if defined(AL_PLATFORM_LINUX)
			return AL::memcmp(
				&address,
				&address2,
				sizeof(Type)
			);
#elif defined(AL_PLATFORM_WINDOWS)
			return AL::memcmp(
				address.u.Word,
				address2.u.Word
			);
#endif
		}
	};

	class IPAddress
	{
		AddressFamilies family;
		
		union
		{
			typename _IPAddress_Helper<AddressFamilies::IPv4>::Type ipv4;
			typename _IPAddress_Helper<AddressFamilies::IPv6>::Type ipv6;
		};

	public:
		static const IPAddress Any;
		static const IPAddress Any6;
		
		static const IPAddress Loopback;
		static const IPAddress Loopback6;

		typedef typename _IPAddress_Helper<AddressFamilies::IPv4>::Type Address;
		typedef typename _IPAddress_Helper<AddressFamilies::IPv6>::Type Address6;

		// @throw AL::Exceptions::Exception
		// @return false if not found
		static bool Resolve(IPAddress& address, const String& hostname)
		{
			addrinfo hint = { 0 };
			hint.ai_family = AF_UNSPEC;

			addrinfo* lpResult;

			if (auto errorCode = getaddrinfo(hostname.GetCString(), "", &hint, &lpResult))
			{
				switch (errorCode)
				{
					case EAI_FAIL:
					case EAI_AGAIN:
						return false;
				}

				throw Exceptions::SocketException(
					"getaddrinfo",
					errorCode
				);
			}

			switch (lpResult->ai_family)
			{
				case _IPAddress_Helper<AddressFamilies::IPv4>::Family:
					address.ipv4 = _IPAddress_Helper<AddressFamilies::IPv4>::GetAddress(
						lpResult->ai_addr
					);
					break;

				case _IPAddress_Helper<AddressFamilies::IPv6>::Family:
					address.ipv6 = _IPAddress_Helper<AddressFamilies::IPv6>::GetAddress(
						lpResult->ai_addr
					);
					break;

				default:
					freeaddrinfo(lpResult);
					throw Exceptions::NotImplementedException();
			}

			freeaddrinfo(lpResult);

			return true;
		}

		// @throw AL::Exceptions::Exception
		static void FromString(IPAddress& address, const String& string)
		{
			auto result = inet_pton(
				_IPAddress_Helper<AddressFamilies::IPv4>::Family,
				string.GetCString(),
				&address.ipv4
			);

			if (result == -1)
			{

				throw Exceptions::SocketException(
					"inet_pton"
				);
			}

			else if (result == 0)
			{
				result = inet_pton(
					_IPAddress_Helper<AddressFamilies::IPv6>::Family,
					string.GetCString(),
					&address.ipv6
				);

				if (result == -1)
				{

					throw Exceptions::SocketException(
						"inet_pton"
					);
				}
				else if (result == 0)
				{

					throw Exceptions::Exception(
						"'%s' is not a valid IPv4 or IPv6 address",
						string.GetCString()
					);
				}
			}
		}

		IPAddress()
			: IPAddress(
				0
			)
		{
		}

		IPAddress(uint32 address)
			: family(
				AddressFamilies::IPv4
			)
		{
			ipv4.s_addr = address;
		}

		IPAddress(const typename _IPAddress_Helper<AddressFamilies::IPv4>::Type& address)
			: family(
				AddressFamilies::IPv4
			),
			ipv4(
				address
			)
		{
		}

		IPAddress(const typename _IPAddress_Helper<AddressFamilies::IPv6>::Type& address)
			: family(
				AddressFamilies::IPv6
			),
			ipv6(
				address
			)
		{
		}

		virtual ~IPAddress()
		{
		}

		bool IsV4() const
		{
			return family == AddressFamilies::IPv4;
		}

		bool IsV6() const
		{
			return family == AddressFamilies::IPv6;
		}

		bool IsV4MappedToV6() const
		{
			if (IsV4())
			{

				return false;
			}

			static constexpr uint8 V4_BUFFER_PREFIX[12] =
			{
				0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0xFF, 0xFF
			};

#if defined(AL_PLATFORM_LINUX)
			return AL::memcmp(
				&ipv6.__in6_u.__u6_addr8[0],
				&V4_BUFFER_PREFIX[0],
				12
			);
#elif defined(AL_PLATFORM_WINDOWS)
			return AL::memcmp(
				&ipv6.u.Byte[0],
				&V4_BUFFER_PREFIX[0],
				12
			);
#endif
		}

		auto GetFamily() const
		{
			return family;
		}

		auto& GetAddress() const
		{
			return ipv4;
		}
		
		auto& GetAddress6() const
		{
			return ipv6;
		}

		bool Cast(IPAddress& address, AddressFamilies family) const
		{
			if (IsV4())
			{
				if (family == AddressFamilies::IPv4)
				{
					address = *this;

					return true;
				}
				else if (family == AddressFamilies::IPv6)
				{
					address.family = AddressFamilies::IPv6;

#if defined(AL_PLATFORM_LINUX)
					AL::memset(
						&address.ipv6.__in6_u.__u6_addr8[0],
						0x00,
						10
					);

					address.ipv6.__in6_u.__u6_addr8[10] = 0xFF;
					address.ipv6.__in6_u.__u6_addr8[11] = 0xFF;

					AL::memcpy(
						&address.ipv6.__in6_u.__u6_addr8[12],
						&ipv4.s_addr,
						4
					);
#elif defined(AL_PLATFORM_WINDOWS)
					memset(
						&address.ipv6.u.Byte[0],
						0x00,
						10
					);

					address.ipv6.u.Byte[10] = 0xFF;
					address.ipv6.u.Byte[11] = 0xFF;

					AL::memcpy(
						&address.ipv6.u.Byte[12],
						&ipv4.s_addr,
						4
					);
#endif

					return true;
				}
			}
			else if (IsV6())
			{
				if ((family == AddressFamilies::IPv4) && IsV4MappedToV6())
				{
					address.family = AddressFamilies::IPv4;

#if defined(AL_PLATFORM_LINUX)
					auto lpAddress = &ipv6.__in6_u.__u6_addr8[12];
#elif defined(AL_PLATFORM_WINDOWS)
					auto lpAddress = &ipv6.u.Byte[12];
#endif

					address.ipv4.s_addr = *reinterpret_cast<const uint32*>(
						lpAddress
					);

					return true;
				}
				else if (family == AddressFamilies::IPv6)
				{
					address = *this;

					return true;
				}
			}

			return false;
		}

		// @throw AL::Exceptions::Exception
		String ToString() const
		{
			auto lpAddress = IsV4() ?
				static_cast<const void*>(&ipv4) :
				static_cast<const void*>(&ipv6);

			auto addressFamily = IsV4() ?
				_IPAddress_Helper<AddressFamilies::IPv4>::Family :
				_IPAddress_Helper<AddressFamilies::IPv6>::Family;

			auto addressStringLength = IsV4() ?
				_IPAddress_Helper<AddressFamilies::IPv4>::StringLength :
				_IPAddress_Helper<AddressFamilies::IPv6>::StringLength;

			String string(
				'\0',
				addressStringLength
			);

			if (!inet_ntop(addressFamily, lpAddress, &string[0], addressStringLength))
			{

				throw Exceptions::SocketException(
					"inet_ntop"
				);
			}

			return string;
		}
	};
}

inline const AL::Sockets::IPAddress AL::Sockets::IPAddress::Any(
#if defined(AL_PLATFORM_LINUX)
	in_addr{ AL::BitConverter::HostToNetwork(INADDR_ANY) }
#elif defined(AL_PLATFORM_WINDOWS)
	in4addr_any
#endif
);
inline const AL::Sockets::IPAddress AL::Sockets::IPAddress::Any6(
	in6addr_any
);

inline const AL::Sockets::IPAddress AL::Sockets::IPAddress::Loopback(
#if defined(AL_PLATFORM_LINUX)
	in_addr{ AL::BitConverter::HostToNetwork(INADDR_LOOPBACK) }
#elif defined(AL_PLATFORM_WINDOWS)
	in4addr_loopback
#endif
);
inline const AL::Sockets::IPAddress AL::Sockets::IPAddress::Loopback6(
	in6addr_loopback
);
