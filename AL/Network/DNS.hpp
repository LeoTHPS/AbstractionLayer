#pragma once
#include "AL/Common.hpp"

#include "IPAddress.hpp"
#include "SocketException.hpp"

#include "AL/Collections/Array.hpp"

#if defined(AL_PLATFORM_PICO_W)
	#include <lwip/dns.h>
#elif defined(AL_PLATFORM_PICO)
	#warning Platform not supported
#elif defined(AL_PLATFORM_LINUX)
	#include <netdb.h>

	#include <arpa/inet.h>
#elif defined(AL_PLATFORM_WINDOWS)
	#include "WinSock.hpp"
#endif

namespace AL::Network
{
	class DNS
	{
		inline static size_t initCount = 0;

		DNS() = delete;

	public:
		static Bool IsInitialized()
		{
			return initCount != 0;
		}

		static size_t GetInitCount()
		{
			return initCount;
		}

		// @throw AL::Exception
		static Void Init()
		{
			if (!IsInitialized())
			{
#if defined(AL_PLATFORM_PICO_W)
				::dns_init();
#elif defined(AL_PLATFORM_LINUX)
				// TODO:implement
				// throw NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
				WinSock::Load();
#endif
			}

			++initCount;
		}

		static Void Deinit()
		{
			if (IsInitialized() && (--initCount == 0))
			{
#if defined(AL_PLATFORM_PICO_W)
				++initCount; // do nothing - this platform can't be cleaned up
#elif defined(AL_PLATFORM_LINUX)
				// TODO: implement
#elif defined(AL_PLATFORM_WINDOWS)
				WinSock::Unload();
#endif
			}
		}

		// @throw AL::Exception
		template<size_t S>
		static Void SetServers(const IPAddress(&addresses)[S])
		{
			AL_ASSERT(
				IsInitialized(),
				"DNS not initialized"
			);

#if defined(AL_PLATFORM_PICO_W)
			static_assert(
				S <= Integer<uint8>::Maximum,
				"S must must be less than or equal to 0xFF"
			);

			Collections::Array<::ip_addr_t> ip_addrs(
				S
			);

			for (size_t i = 0; i < S; ++i)
			{
				ip_addrs[i] = addresses[i].ToNative();
			}

			::dns_setserver(
				S,
				&ip_addrs[0]
			);
#elif defined(AL_PLATFORM_LINUX)
			// TODO: implement
			throw NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			// TODO: implement
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		// @return AL::False if not found
		static Bool Resolve(IPAddress& address, const String& hostname)
		{
			AL_ASSERT(
				IsInitialized(),
				"DNS not initialized"
			);

#if defined(AL_PLATFORM_PICO_W)
			struct ResolveContext
			{
				Bool        Success;
				Bool        Complete;
				::ip_addr_t ip_addr;
			};

			ResolveContext context =
			{
				.Success  = False,
				.Complete = False
			};

			::dns_found_callback callback = [](const char* _name, const ::ip_addr_t* _ip_addr, void* _lpParam)
			{
				auto lpContext = reinterpret_cast<ResolveContext*>(
					_lpParam
				);

				if (_ip_addr == nullptr)
				{
					lpContext->Success  = False;
					lpContext->Complete = True;

					return;
				}

				lpContext->ip_addr = *_ip_addr;

				lpContext->Success  = True;
				lpContext->Complete = True;
			};

			ErrorCode errorCode;

			if ((errorCode = ::dns_gethostbyname_addrtype(hostname.GetCString(), &context.ip_addr, callback, &context, LWIP_DNS_ADDRTYPE_IPV4_IPV6)) != ::ERR_OK)
			{
				if (errorCode != ::ERR_INPROGRESS)
				{

					throw SocketException(
						"dns_gethostbyname_addrtype",
						errorCode
					);
				}

				while (!context.Complete)
				{
					Sleep(
						TimeSpan::FromMilliseconds(10)
					);
				}
			}

			address = IPAddress::FromNative(
				context.ip_addr
			);
#elif defined(AL_PLATFORM_LINUX) || defined(AL_PLATFORM_WINDOWS)
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
					address = reinterpret_cast<const ::sockaddr_in*>(lpResult->ai_addr)->sin_addr;
					break;

				case AF_INET6:
					address = reinterpret_cast<const ::sockaddr_in6*>(lpResult->ai_addr)->sin6_addr;
					break;

				default:
					::freeaddrinfo(lpResult);
					throw OperationNotSupportedException();
			}

			::freeaddrinfo(
				lpResult
			);
#endif

			return True;
		}
	};
}
