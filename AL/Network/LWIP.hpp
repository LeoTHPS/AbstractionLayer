#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_PICO_W)
	#warning Platform not supported
#endif

#include "AL/Hardware/PicoW/CYW43.hpp"

namespace AL::Network
{
	class LWIP
	{
		LWIP() = delete;

	public:
		static Void Poll()
		{
			Hardware::PicoW::CYW43::Poll();
		}

		template<typename F, typename ... TArgs>
		static AL_INLINE Void Sync(F&& function, TArgs ... args)
		{
			Sync_Begin();

			try
			{
				function(
					Forward<TArgs>(args) ...
				);
			}
			catch (...)
			{
				Sync_End();

				throw;
			}

			Sync_End();
		}

		static AL_INLINE Void Sync_Begin()
		{
#if defined(PICO_CYW43_ARCH_THREADSAFE_BACKGROUND)
			::cyw43_arch_lwip_begin();
#endif
		}

		static AL_INLINE Void Sync_End()
		{
#if defined(PICO_CYW43_ARCH_THREADSAFE_BACKGROUND)
			::cyw43_arch_lwip_end();
#endif
		}
	};
}
