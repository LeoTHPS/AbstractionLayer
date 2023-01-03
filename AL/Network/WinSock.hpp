#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_WINDOWS)
	#error Platform not supported
#endif

#include "SocketException.hpp"

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <MSWSock.h>

#if defined(AL_COMPILER_MSVC)
	#pragma comment(lib, "Ws2_32.lib")
#endif

namespace AL::Network
{
	class WinSock
	{
		inline static ::WSADATA data;
		inline static size_t    loadCount = 0;

		WinSock() = delete;

	public:
		static Bool IsLoaded()
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

		// @throw AL::Exception
		static Void Load()
		{
			if (++loadCount == 1)
			{
				if (::WSAStartup(MAKEWORD(2, 2), &data) != NO_ERROR)
				{

					throw SocketException(
						"WSAStartup"
					);
				}
			}
		}

		static Bool TryLoad()
		{
			try
			{
				Load();
			}
			catch (const Exception&)
			{

				return False;
			}

			return True;
		}

		static Void Unload()
		{
			if (GetLoadCount() && !--loadCount)
			{

				::WSACleanup();
			}
		}
	};
}
