#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_WINDOWS)
	#error Platform not supported
#endif

#include "AL/Exceptions/SocketException.hpp"

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <MSWSock.h>

#pragma comment(lib, "Ws2_32.lib")

namespace AL::Sockets
{
	class WinSock
	{
		inline static WSADATA data;
		
		inline static size_t loadCount = 0;

		WinSock() = delete;

	public:
		static bool IsLoaded()
		{
			return loadCount != 0;
		}

		static auto& GetData()
		{
			return data;
		}

		static auto GetLoadCount()
		{
			return loadCount;
		}

		// @throw AL::Exceptions::Exception
		static void Load()
		{
			if (++loadCount == 1)
			{
				if (WSAStartup(MAKEWORD(2, 2), &data) != NO_ERROR)
				{

					throw Exceptions::SocketException(
						"WSAStartup"
					);
				}
			}
		}

		static void Unload()
		{
			if (--loadCount == 0)
			{

				WSACleanup();
			}
		}
	};
}
