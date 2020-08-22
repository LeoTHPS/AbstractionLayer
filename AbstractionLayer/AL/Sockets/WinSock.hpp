#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_WINDOWS)
	#error Platform not supported
#endif

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <MSWSock.h>

#pragma comment(lib, "Ws2_32.lib")

namespace AL::Sockets
{
	class WinSock
	{
		WSADATA data;
		
		bool isLoaded = false;

		WinSock(const WinSock&) = delete;

	public:
		WinSock()
		{
		}

		WinSock(WinSock&& winsock)
			: data(
				Move(winsock.data)
			),
			isLoaded(
				winsock.isLoaded
			)
		{
			winsock.isLoaded = false;
		}

		virtual ~WinSock()
		{
			if (IsLoaded())
			{
				Unload();
			}
		}

		bool IsLoaded() const
		{
			return isLoaded;
		}

		// @throw AL::Exceptions::Exception
		void Load()
		{
			if (IsLoaded())
			{
				
				throw Exceptions::Exception(
					"WinSock already loaded"
				);
			}

			if (WSAStartup(MAKEWORD(2, 2), &data) != NO_ERROR)
			{

				throw Exceptions::SocketException(
					"WSAStartup"
				);
			}

			isLoaded = true;
		}

		void Unload()
		{
			if (IsLoaded())
			{
				WSACleanup();

				isLoaded = false;
			}
		}

		auto& operator = (WinSock&& winsock)
		{
			data = Move(
				winsock.data
			);

			isLoaded = winsock.isLoaded;
			winsock.isLoaded = false;

			return *this;
		}
	};
}
